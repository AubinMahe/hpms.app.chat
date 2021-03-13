package hpms.app.chat.jfx;

import javafx.geometry.Pos;
import javafx.scene.paint.Color;

public final class Message {

   public final String _text;
   public final Pos    _alignment;
   public final Color  _color;

   public Message( String text, Pos alignment, Color color ) {
      _text      = text;
      _alignment = alignment;
      _color     = color;
   }
}
