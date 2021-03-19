package hpms.app.chat;

import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.util.Map;
import java.util.prefs.Preferences;

import javax.imageio.ImageIO;

import hpms.app.chat.jfx.ChatCtrl;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

public class Main extends Application {

   private static final String LOCK_FILENAME = System.getProperty( "user.home" ) + "/." + Main.class.getPackage().getName() + ".lock";
   private static final String APP_NAME      = "Bavardages";

   private static final String CLI_USER        = "user";
   private static final String CLI_ADDRESS     = "address";
   private static final String CLI_PORT        = "port";
   private static final String CLI_HELP        = "--help";
   private static final String CLI_HELP_SHORT1 = "-h";
   private static final String CLI_HELP_SHORT2 = "-?";
   private static final String CLI_DEBUG       = "--debug";
   private static final String CLI_DEBUG_SHORT = "-d";

   private static final String DEFAULT_USERNAME = System.getProperty( "user.name" );
   private static final int    DEFAULT_PORT     = 2416;
   private static final String DEFAULT_ADDRESS  = "239.0.0.0";

   private Stage    _mainWindow;
   private ChatCtrl _ctrl;

   private void minimizeToTray() {
      try {
         final Preferences prefs = Preferences.userNodeForPackage( getClass());
         final double x = _mainWindow.getX();
         final double y = _mainWindow.getY();
         final double w = _mainWindow.getWidth();
         final double h = _mainWindow.getHeight();
         prefs.putDouble( "x", x );
         prefs.putDouble( "y", y );
         prefs.putDouble( "w", w );
         prefs.putDouble( "h", h );
      }
      catch( final Throwable t ) {/**/}
   }

   private void showWindow() {
      try {
         _mainWindow.show();
         _mainWindow.toFront();
      }
      catch( final Throwable t ) {
         t.printStackTrace();
      }
   }

   private void quit( java.awt.SystemTray tray, java.awt.TrayIcon trayIcon ) {
      tray.remove( trayIcon );
      _ctrl.leave();
      System.exit( 0 );
   }

   private static void usage() {
      System.err.printf( "usage: java --modules=<...> -m %s/%s <options>\n"
         + "options are:\n"
         + "\t--%s=<user name> default is %s,\n"
         + "\t--%s=<multicast address> default is %s\n"
         + "\t--%s=nnnn default is %d\n"
         + "\t%s or %s or %s for help\n"
         + "\t%s or %s for debug\n",
         Main.class.getPackage().getName(), Main.class.getName(),
         CLI_USER   , DEFAULT_USERNAME,
         CLI_ADDRESS, DEFAULT_ADDRESS,
         CLI_PORT   , DEFAULT_PORT,
         CLI_HELP, CLI_HELP_SHORT1, CLI_HELP_SHORT2,
         CLI_DEBUG, CLI_DEBUG_SHORT );
      System.exit( 0 );
   }

   @Override
   public void start( final Stage mainStage ) throws Exception {
      _mainWindow = mainStage;
      boolean debug = false;
      for( final String arg : getParameters().getUnnamed()) {
         if( arg.equals( CLI_HELP ) || arg.equals( CLI_HELP_SHORT1 ) || arg.equals( CLI_HELP_SHORT2 )) {
            usage();
            return;
         }
         else if( arg.equals( CLI_DEBUG ) || arg.equals( CLI_DEBUG_SHORT )) {
            debug = true;
         }
      }
      final Map<String, String> args  = getParameters().getNamed();
      final String user  = args.getOrDefault( CLI_USER   , DEFAULT_USERNAME );
      final String mcast = args.getOrDefault( CLI_ADDRESS, DEFAULT_ADDRESS );
      final int    port  = Integer.parseInt( args.getOrDefault( CLI_PORT, Integer.toString( DEFAULT_PORT )));
      final FXMLLoader loader  = new FXMLLoader( ChatCtrl.class.getResource( "ChatView.fxml" ));
      final Parent     view    = loader.load();
      final Scene      scene   = new Scene( view );
      _ctrl = loader.getController();
      _ctrl.setDebug( debug );
      _ctrl.join( user, mcast, port );
      _mainWindow.setTitle( APP_NAME + " - " + user );
      _mainWindow.setOnHiding      ( e -> minimizeToTray());
      _mainWindow.setOnCloseRequest( e -> minimizeToTray());
      _mainWindow.setScene( scene );
      _mainWindow.getIcons().add( new Image( getClass().getResource( "icon_256x256.png" ).toExternalForm()));
      final Preferences prefs = Preferences.userNodeForPackage( getClass());
      final double x = prefs.getDouble( "x", Double.NaN );
      final double y = prefs.getDouble( "y", Double.NaN );
      final double w = prefs.getDouble( "w", Double.NaN );
      final double h = prefs.getDouble( "h", Double.NaN );
      if( ! Double.isNaN( x )) {
         _mainWindow.setX( x );
         _mainWindow.setY( y );
         _mainWindow.setWidth( w );
         _mainWindow.setHeight( h );
      }
      //--
      //-- System Tray --
      //--
      try {
         java.awt.Toolkit.getDefaultToolkit();
         final java.awt.SystemTray       tray        = java.awt.SystemTray.getSystemTray();
         final URL                       icoURL      = getClass().getResource( "icon_24x24.png" );
         final java.awt.Image            image       = ImageIO.read( icoURL );
         final java.awt.TrayIcon         trayIcon    = new java.awt.TrayIcon( image );
         final java.awt.MenuItem         openItem    = new java.awt.MenuItem( "Ouvrir 'Bavardage'" );
         final java.awt.CheckboxMenuItem showOnRecv  = new java.awt.CheckboxMenuItem( "Ouvrir sur réception de messages" );
         final java.awt.MenuItem         quitItem    = new java.awt.MenuItem( "Fermer 'Bavardage'" );
         final java.awt.Font             defaultFont = java.awt.Font.decode(null);
         final java.awt.Font             boldFont    = defaultFont.deriveFont(java.awt.Font.BOLD);
         final java.awt.PopupMenu        popup       = new java.awt.PopupMenu();
         trayIcon  .addActionListener( event -> Platform.runLater(() -> showWindow()));
         openItem  .addActionListener( event -> Platform.runLater(() -> showWindow()));
         showOnRecv.addActionListener( event -> Platform.runLater(() -> _ctrl.setShowMainWindowOnReceive( showOnRecv.getState())));
         quitItem  .addActionListener( event -> quit( tray, trayIcon ));
         openItem  .setFont( boldFont );
         popup.add( openItem );
         popup.add( showOnRecv );
         popup.add( quitItem );
         trayIcon.setPopupMenu( popup );
         tray.add( trayIcon );
         showOnRecv.setState( _ctrl.showMainWindowOnReceiveProperty().get());
         _ctrl.showMainWindowOnReceiveProperty().addListener(( o, b, a ) -> showOnRecv.setState( a ));
         _ctrl.usernameProperty().addListener(( o, b, a ) -> {
            if( a == null || a.trim().isEmpty()) {
               _ctrl.usernameProperty().set( DEFAULT_USERNAME );
            }
            else {
               _mainWindow.setTitle( APP_NAME + " - " + a );
            }
         });
      }
      catch( final Throwable t ) {
         t.printStackTrace();
      }
   }

   @SuppressWarnings("resource")
   public static void main( String[] args ) throws IOException {
      final FileOutputStream fos          = new FileOutputStream( LOCK_FILENAME );
      final FileChannel      channel      = fos.getChannel();
      final FileLock         lock         = channel.tryLock();
      Runtime.getRuntime().addShutdownHook( new Thread() { @Override public void run() {
         try {
            if( lock != null ) {
               lock.release();
               lock.close();
            }
            channel.close();
            fos    .close();
            System.err.println( "Done." );
         }
         catch( final IOException e ) {
            e.printStackTrace();
         }
      }});
      if( lock == null ) {
         System.err.println( "L'application '" + APP_NAME + "' ne doit être lancée qu'une fois par utilisateur." );
         System.exit( 1 );
      }
      else {
         Platform.setImplicitExit( false );
         Application.launch( args );
      }
   }
}
