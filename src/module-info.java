module hpms.app.chat {

   requires transitive java.prefs;
   requires transitive java.xml;
   requires transitive java.desktop;

   requires transitive javafx.base;
   requires transitive javafx.controls;
   requires transitive javafx.fxml;
   requires transitive javafx.graphics;
   requires transitive javafx.swing;
   requires transitive javafx.web;

   requires transitive jdk.jsobject;
   requires transitive jdk.xml.dom;

   exports hpms.app.chat;
   opens   hpms.app.chat     to javafx.graphics;

   exports hpms.app.chat.jfx;
   opens   hpms.app.chat.jfx to javafx.fxml;
}
