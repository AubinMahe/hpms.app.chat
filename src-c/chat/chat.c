#include "chat.h"

#include <utils/set_of_strings.h>
#include <utils/net_buff.h>
#include <utils/preferences.h>

#include <ifaddrs.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>

#define USERNAME_MAX    (32+1)
#define PSEUDO_MAX      40
#define MCAST_MIN        9
#define MCAST_MAX       11
#define PAYLOAD_MAX     (64*1024)

static const char * JOIN_MSG  = "a rejoint la conversation.";
static const char * LEAVE_MSG = "a quitté la conversation.";

typedef struct {
   chat_message_consumer consumer;
   int                   sckt;
   char                  username[USERNAME_MAX];
   chat_verbosity        verbosity;
   struct sockaddr_in    recv_addr;
   struct sockaddr_in    send_addr;
   struct ip_mreq        imr;
   bool                  is_alive;
   char                  localID[USERNAME_MAX+1+15+1+5];
   char                  pseudo[PSEUDO_MAX];
   set_of_strings        friends;
   net_buff              recv_buff;
   net_buff              send_buff;
   pthread_t             thread;
} chat_private;

bool chat_new( chat * ac, chat_message_consumer consumer ) {
   if( ac == NULL ) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   *ac = malloc( sizeof( chat_private ));
   if( *ac == NULL ) {
      return false;
   }
   chat_private * This = (chat_private *)*ac;
   memset( This, 0, sizeof( chat_private ));
#ifdef _WIN32
   WSADATA wsaData;
   if( WSAStartup( 0x0101, &wsaData )) {
      perror( "WSAStartup" );
      return false;
   }
#endif
   This->consumer = consumer;
   This->sckt     = -1;
   const char * user = getenv( "USER" );
   if( user ) {
      snprintf( This->username, USERNAME_MAX, "%s", user );
   }
   else {
      snprintf( This->username, USERNAME_MAX, "%s", "???" );
   }
   if( ! set_of_strings_new( &This->friends )) {
      free( This );
      *ac = NULL;
      return false;
   }
   if( ! net_buff_new( &This->recv_buff, PAYLOAD_MAX )) {
      set_of_strings_delete( &This->friends );
      free( This );
      *ac = NULL;
      return false;
   }
   if( ! net_buff_new( &This->send_buff, PAYLOAD_MAX )) {
      net_buff_delete( &This->recv_buff );
      set_of_strings_delete( &This->friends );
      free( This );
      *ac = NULL;
      return false;
   }
   preferences  prefs;
   const char * pseudo  = NULL;
   const char * address = NULL;
   unsigned short port;
   if(   preferences_load( &prefs, ".chatrc" )
      && preferences_get_string( prefs, "pseudo" , &pseudo  )
      && preferences_get_string( prefs, "address", &address )
      && preferences_get_ushort( prefs, "port"   , &port    ))
   {
      preferences_get_int( prefs, "verbosity", (int *)&This->verbosity );
      const char * prefs_path = NULL;
      if( preferences_get_path( prefs, &prefs_path)) {
         fprintf( stderr, "Preferences read from %s\n", prefs_path );
      }
      chat_join( *ac, pseudo, address, port );
   }
   return true;
}

bool chat_set_verbosity( chat ac, chat_verbosity verbosity ) {
   if( ac == NULL ) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   chat_private * This = (chat_private *)ac;
   This->verbosity = verbosity;
   return true;
}

static bool get_multicast_interface_address( char * address ) {
   struct ifaddrs * ifaddr = NULL;
   if( getifaddrs( &ifaddr )) {
      perror( "getifaddrs" );
      return false;
   }
   unsigned mask = IFF_MULTICAST | IFF_UP;
   for( struct ifaddrs * ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next ) {
      if((( ifa->ifa_flags & mask ) == mask )&&( ifa->ifa_addr->sa_family == AF_INET )) {
         struct sockaddr_in * addr = (struct sockaddr_in *)ifa->ifa_addr;
         inet_ntop( AF_INET, &addr->sin_addr, address, INET_ADDRSTRLEN );
         break;
      }
   }
   freeifaddrs( ifaddr );
   return true;
}

static void * chat_receive_thread( void * arg ) {
   chat_private * This = (chat_private *)arg;
   This->is_alive = true;
   chat_send((chat)This, JOIN_MSG );
   while( This->is_alive ) {
      net_buff_clear( This->recv_buff );
      if( net_buff_receive( This->recv_buff, This->sckt, &This->recv_addr )) {
         net_buff_flip( This->recv_buff );
         if( This->verbosity == DETAILS ) {
            size_t limit = 0;
            if( net_buff_get_limit( This->recv_buff, &limit )) {
               fprintf( stderr, "%s|packet received, %ld bytes\n", __func__, limit );
            }
         }
         char username[USERNAME_MAX];
         if( ! net_buff_decode_string( This->recv_buff, username, sizeof( username ))) {
            continue;
         }
         if( This->verbosity == DETAILS ) {
            fprintf( stderr, "%s|username: %s\n", __func__, username );
         }
         char remoteID[USERNAME_MAX+1+15+1+5];
         char ipv4[INET_ADDRSTRLEN];
         if( NULL == inet_ntop( AF_INET, &This->recv_addr.sin_addr, ipv4, sizeof( ipv4 ))) {
            perror( "inet_ntop" );
            continue;
         }
         snprintf( remoteID, sizeof( remoteID ), "%s@%s:%d", username, ipv4, This->recv_addr.sin_port );
         if( This->verbosity == DETAILS ) {
            fprintf( stderr, "%s|remoteID: %s\n", __func__, remoteID );
         }
         if( strcmp( remoteID, This->localID )) {
            char pseudo[PSEUDO_MAX];
            if( ! net_buff_decode_string( This->recv_buff, pseudo, sizeof( pseudo ))) {
               continue;
            }
            if( This->verbosity == DETAILS ) {
               fprintf( stderr, "%s|remote pseudo: '%s'\n", __func__, pseudo );
            }
            char message[PAYLOAD_MAX];
            if( ! net_buff_decode_string( This->recv_buff, message, sizeof( message ))) {
               continue;
            }
            if( This->verbosity == DETAILS ) {
               fprintf( stderr, "%s|message: '%s'\n", __func__, message );
            }
            if( 0 == strcmp( JOIN_MSG, message )) {
               if( set_of_strings_contains( This->friends, remoteID )) {
                  if( This->verbosity == DETAILS ) {
                     fprintf( stderr, "%s|message discarded because it's already seen before.\n", __func__);
                  }
               }
               else {
                  set_of_strings_add( This->friends, remoteID );
                  if( This->verbosity == DETAILS ) {
                     fprintf( stderr, "%s|new friend discovered: '%s'.\n", __func__, remoteID );
                  }
                  chat_send((chat)This, JOIN_MSG );
                  This->consumer( pseudo, message );
               }
            }
            else {
               if( 0 == strcmp( LEAVE_MSG, message )) {
                  if( This->verbosity == DETAILS ) {
                     fprintf( stderr, "%s|friend %s has leaved the group.\n", __func__, remoteID );
                  }
                  set_of_strings_remove( This->friends, remoteID );
               }
               This->consumer( pseudo, message );
            }
         }
      }
   }
   return NULL;
}

bool chat_join( chat ac, const char * pseudo, const char * mcast, unsigned short port ) {
   if(( ac == NULL )||( pseudo == NULL )||( mcast == NULL )) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   if( strlen( pseudo ) > PSEUDO_MAX ) {
      fprintf( stderr, "%s: too long pseudo (length > %d)\n", __func__, PSEUDO_MAX );
      return false;
   }
   if( strlen( mcast ) < MCAST_MIN ) {
      fprintf( stderr, "%s: multicast IP v4 address too short: %s, expected 239.0.0.[0..255]\n", __func__, mcast );
      return false;
   }
   if( strlen( mcast ) > MCAST_MAX ) {
      fprintf( stderr, "%s: multicast IP v4 address too long: %s, expected 239.0.0.[0..255]\n", __func__, mcast );
      return false;
   }
   chat_private * This = (chat_private *)ac;
   strcpy( This->pseudo, pseudo );
   memset( &This->recv_addr, 0, sizeof( This->recv_addr ));
   This->recv_addr.sin_family      = AF_INET;
   This->recv_addr.sin_port        = htons( port );
   This->recv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
   memset( &This->send_addr, 0, sizeof( This->send_addr ));
   This->send_addr.sin_family      = AF_INET;
   This->send_addr.sin_port        = htons( port );
   This->send_addr.sin_addr.s_addr = inet_addr( mcast );
   memset( &This->imr, 0, sizeof( This->imr ));
   This->imr.imr_multiaddr.s_addr = inet_addr( mcast );
   This->imr.imr_interface.s_addr = htonl( INADDR_ANY );
   This->sckt = socket( PF_INET, SOCK_DGRAM, 0 );
   if( This->sckt < 0 ) {
      perror( "socket( PF_INET, SOCK_DGRAM )" );
      return false;
   }
   unsigned yes = 1;
   if( setsockopt( This->sckt, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof( yes )) < 0 ) {
      perror( "setsockopt( SOL_SOCKET, SO_REUSEADDR )" );
      return false;
   }
   if( bind( This->sckt, (struct sockaddr*) &This->recv_addr, sizeof( This->recv_addr )) < 0 ) {
      perror( "bind" );
      return false;
   }
   if( setsockopt( This->sckt, IPPROTO_IP, IP_ADD_MEMBERSHIP, &This->imr, sizeof( This->imr )) < 0 ) {
      perror( "setsockopt( IP_ADD_MEMBERSHIP )" );
      return false;
   }
   const char * user = getenv( "USER" );
   if( user == NULL ) {
      fprintf( stderr, "%s|getenv(\"USER\") returns NULL!\n", __func__ );
      return false;
   }
   char ipv4[INET_ADDRSTRLEN];
   get_multicast_interface_address( ipv4 );
   snprintf( This->localID, sizeof( This->localID ), "%s@%s:%d", user, ipv4, This->recv_addr.sin_port );
   This->is_alive = false;
   if( pthread_create( &This->thread, NULL, chat_receive_thread, This )) {
      perror( "pthread_create" );
      return false;
   }
   return true;
}

bool chat_is_alive( chat ac ) {
   if( ac == NULL ) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   chat_private * This = (chat_private *)ac;
   return This->is_alive;
}

bool chat_send( chat ac, const char * message ) {
   if( ac == NULL ) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   chat_private * This = (chat_private *)ac;
   if( ! This->is_alive ) {
      fprintf( stderr, "%s: You must call 'join' first.\n", __func__ );
      return false;
   }
   if( This->verbosity == DETAILS ) {
      fprintf( stderr, "%s|payload <== encode( username='%s', pseudo='%s', message='%s' )\n",
         __func__, This->username, This->pseudo, message );
   }
   return net_buff_clear        ( This->send_buff )
      &&  net_buff_encode_string( This->send_buff, This->username )
      &&  net_buff_encode_string( This->send_buff, This->pseudo   )
      &&  net_buff_encode_string( This->send_buff, message        )
      &&  net_buff_flip         ( This->send_buff )
      &&  net_buff_send         ( This->send_buff, This->sckt, &This->send_addr );
}

bool chat_leave( chat ac ) {
   if( ac == NULL ) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   chat_private * This = (chat_private *)ac;
   if( ! This->is_alive ) {
      fprintf( stderr, "%s: You must call 'join' first.\n", __func__ );
      return false;
   }
   chat_send( ac, LEAVE_MSG );
   if( setsockopt( This->sckt, IPPROTO_IP, IP_DROP_MEMBERSHIP, &This->imr, sizeof( This->imr )) < 0 ) {
      perror( "setsockopt( IP_DROP_MEMBERSHIP )" );
      return false;
   }
   This->is_alive = false;
   void * retVal = NULL;
   pthread_join( This->thread, &retVal );
   return set_of_strings_clear( This->friends );
}

bool chat_close( chat * ac ) {
   if( ac == NULL ) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   chat_private * This = *(chat_private **)ac;
   if( This != NULL ) {
      if( This->is_alive ) {
         chat_leave( *ac );
      }
      set_of_strings_delete( &This->friends );
      net_buff_delete( &This->recv_buff );
      net_buff_delete( &This->send_buff );
      free( *ac );
      *ac = NULL;
   }
   return true;
}
