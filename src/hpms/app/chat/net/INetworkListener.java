package hpms.app.chat.net;

public interface INetworkListener {

   void onMessageReceived( String author, String text );
}
