package hpms.app.chat.net;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.InterfaceAddress;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Set;

public class Network implements Runnable {

   private static final String JOIN_MSG  = "a rejoint la conversation.";
   private static final String LEAVE_MSG = "a quitté la conversation.";

   public static boolean isJoin( String message ) {
      return message.equals( JOIN_MSG );
   }

   private final INetworkListener _listener;
   private /* */ boolean          _debug;
   private /* */ String           _username;
   private /* */ String           _localID;
   private /* */ String           _pseudo;
   private /* */ MulticastSocket  _socket;
   private /* */ SocketAddress    _group;
   private /* */ NetworkInterface _intrfc;

   public Network( INetworkListener listener ) {
      _listener = listener;
   }

   public void setDebug( boolean debug ) {
      _debug = debug;
   }

   public void join( String pseudo, String mcast, int port ) throws IOException {
      if( _socket != null ) {
         if( _debug ) {
            System.err.printf( "%s.join|Socket is null, exiting.\n", getClass().getName());
         }
         return;
      }
      _pseudo = pseudo;
      _socket = new MulticastSocket( port );
      _group  = new InetSocketAddress( InetAddress.getByName( mcast ), port );
      _intrfc = null;
      for( final Enumeration<NetworkInterface> e = NetworkInterface.getNetworkInterfaces(); e.hasMoreElements(); ) {
         final NetworkInterface intrfc = e.nextElement();
         if( intrfc.isUp() && intrfc.supportsMulticast()) {
            _intrfc = intrfc;
            break;
         }
      }
      if( _intrfc == null ) {
         if( _debug ) {
            System.err.printf( "%s.join|No multicast capable interface found!\n", getClass().getName());
         }
         throw new IOException( "No multicast capable interface found!" );
      }
      _username = System.getProperty( "user.name" );
      _localID  = _username + "@";
      for( final InterfaceAddress ifaceAddr :_intrfc.getInterfaceAddresses()) {
         if(( ifaceAddr.getBroadcast() != null )&& ifaceAddr.getAddress().isSiteLocalAddress()) {
            _localID += ifaceAddr.getAddress().getHostAddress() + ":" + port;
            if( _debug ) {
               System.err.printf( "%s.join|localID: %s\n", getClass().getName(), _localID );
            }
            break;
         }
      }
      _socket.joinGroup( _group, _intrfc );
      final Thread t = new Thread( this );
      t.setName( getClass().getName());
      t.setDaemon( true );
      t.start();
      send( JOIN_MSG );
   }

   public void send( String line ) {
      try {
         if( _debug ) {
            System.err.printf( "%s.send|%s/%s/%s\n", getClass().getName(), _username, _pseudo, line );
         }
         final byte[] usernameBytes = _username.getBytes( StandardCharsets.UTF_8 );
         final byte[] pseudoBytes = _pseudo.getBytes( StandardCharsets.UTF_8 );
         final byte[] lineBytes = line.getBytes( StandardCharsets.UTF_8 );
         final ByteBuffer data = ByteBuffer.allocate( 4 + usernameBytes.length + 4 + pseudoBytes.length + 4 + lineBytes.length );
         data.putInt( usernameBytes.length );
         data.put   ( usernameBytes );
         data.putInt( pseudoBytes.length );
         data.put   ( pseudoBytes );
         data.putInt( lineBytes.length );
         data.put   ( lineBytes );
         final DatagramPacket packet = new DatagramPacket( data.array(), data.position(), _group );
         _socket.send( packet );
      }
      catch( final Throwable t ) {
         t.printStackTrace();
      }
   }

   @Override
   public void run() {
      try {
         final byte[]         data    = new byte[64*1024];
         final ByteBuffer     decoder = ByteBuffer.wrap( data );
         final DatagramPacket packet  = new DatagramPacket( data, data.length );
         final Set<String>    friends = new HashSet<>();
         while( _socket != null ) {
            decoder.clear();
            _socket.receive( packet );
            if( _debug ) {
               System.err.printf( "%s.run|packet received, %d bytes\n", getClass().getName(), packet.getLength());
            }
            decoder.limit( packet.getLength());
            final int usernameLength = decoder.getInt();
            if( _debug ) {
               System.err.printf( "%s.run|usernameLength: %d\n", getClass().getName(), usernameLength );
            }
            final String username = new String( data, decoder.position(), usernameLength, StandardCharsets.UTF_8 );
            if( _debug ) {
               System.err.printf( "%s.run|username: %s\n", getClass().getName(), username );
            }
            final String remoteID = username + "@" + packet.getAddress().getHostName() + ":" + packet.getPort();
            if( _debug ) {
               System.err.printf( "%s.run|remoteID: %s\n", getClass().getName(), remoteID );
            }
            if( ! remoteID.equals( _localID )) {
               decoder.position( decoder.position() + usernameLength );
               final int    pseudoLength = decoder.getInt();
               if( _debug ) {
                  System.err.printf( "%s.run|pseudoLength: %d\n", getClass().getName(), pseudoLength );
               }
               final String pseudo = new String( data, decoder.position(), pseudoLength, StandardCharsets.UTF_8 );
               if( _debug ) {
                  System.err.printf( "%s.run|pseudo: %s\n", getClass().getName(), pseudo );
               }
               decoder.position( decoder.position() + pseudoLength );
               final int lineLength = decoder.getInt();
               if( _debug ) {
                  System.err.printf( "%s.run|lineLength: %d\n", getClass().getName(), lineLength );
               }
               final String message = new String( data, decoder.position(), lineLength, StandardCharsets.UTF_8 );
               if( _debug ) {
                  System.err.printf( "%s.run|message: %s\n", getClass().getName(), message );
               }
               if( message.equals( JOIN_MSG )) {
                  if( friends.contains( remoteID )) {
                     if( _debug ) {
                        System.err.printf( "%s.run|message discarded because it's already seen before.\n", getClass().getName());
                     }
                  }
                  else {
                     friends.add( remoteID );
                     if( _debug ) {
                        System.err.printf( "%s.run|new friend discovered: '%s'.\n", getClass().getName(), remoteID );
                     }
                     send( JOIN_MSG );
                     _listener.onMessageReceived( pseudo, message );
                  }
               }
               else {
                  if( message.equals( LEAVE_MSG )) {
                     if( _debug ) {
                        System.err.printf( "%s.run|friend %s has leaved the group.\n", getClass().getName(), remoteID );
                     }
                     friends.remove( remoteID );
                  }
                  _listener.onMessageReceived( pseudo, message );
               }
            }
         }
      }
      catch( final Throwable t ) {
//         t.printStackTrace();
      }
   }

   public void leave() {
      try {
         if( _socket == null ) {
            if( _debug ) {
               System.err.printf( "%s.leave|socket is null.\n", getClass().getName());
            }
            return;
         }
         send( LEAVE_MSG );
         _socket.leaveGroup( _group, _intrfc );
         _socket.close();
         _socket = null;
         if( _debug ) {
            System.err.printf( "%s.leave|group leaved.\n", getClass().getName());
         }
      }
      catch( final Throwable t ) {
         t.printStackTrace();
      }
   }
}
