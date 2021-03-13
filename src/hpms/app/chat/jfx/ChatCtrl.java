package hpms.app.chat.jfx;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import hpms.app.chat.net.INetworkListener;
import hpms.app.chat.net.Network;
import javafx.application.Platform;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.StringProperty;
import javafx.fxml.FXML;
import javafx.geometry.Pos;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class ChatCtrl implements INetworkListener {

   private static final Color COLORS[] = {
      Color.DARKSALMON,
      Color.DARKGREEN,
      Color.ORANGERED,
      Color.INDIGO,
   };

   @FXML private TextField         _username;
   @FXML private CheckBox          _showOnRecv;
   @FXML private ListView<Message> _conversation;
   @FXML private TextField         _message;

   private final Map<String, Color> _authorsColor = new HashMap<>();
   private final Network            _network      = new Network( this );
   private /* */ int                _colorIndex;
   private /* */ boolean            _showMainWindowOnReceive = true;

   @FXML
   private void initialize() {
      _message.setOnAction( evt -> {
         final String myText = _message.getText();
         _conversation.getItems().add( new Message( myText, Pos.CENTER_LEFT, Color.DARKBLUE ));
         _network.send( myText );
         _message.setText( null );
      });
      _showOnRecv.selectedProperty().set( _showMainWindowOnReceive );
   }

   public void setDebug( boolean debug ) {
      _network.setDebug( debug );
   }

   public BooleanProperty showMainWindowOnReceiveProperty() {
      return _showOnRecv.selectedProperty();
   }

   public StringProperty usernameProperty() {
      return _username.textProperty();
   }

   public void setShowMainWindowOnReceive( boolean show ) {
      _showMainWindowOnReceive = show;
      _showOnRecv.setSelected( show );
   }

   public void join( String user, String mcast, int port ) throws IOException {
      _username.setText( user );
      _network.join( user, mcast, port );
      _message.requestFocus();
   }

   private void messageReceived( String author, String text ) {
      Color color = _authorsColor.get( author );
      if( color == null ) {
         color = COLORS[_colorIndex++];
         _colorIndex %= COLORS.length;
         _authorsColor.put( author, color );
      }
      _conversation.getItems().add( new Message( author + ": " + text, Pos.CENTER_RIGHT, color ));
      if( _showMainWindowOnReceive ) {
         final Stage mainWindow = (Stage)_message.getScene().getWindow();
         mainWindow.show();
         mainWindow.toFront();
      }
   }

   @Override
   public void onMessageReceived( String author, String text ) {
      Platform.runLater(() -> messageReceived( author, text));
   }

   public void leave() {
      _network.leave();
   }
}
