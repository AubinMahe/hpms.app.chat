package hpms.app.chat.jfx;

import javafx.scene.control.Label;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.layout.HBox;
import javafx.util.Callback;

public class ColoredCellFactory implements Callback<ListView<Message>, ListCell<Message>> {

   @Override
   public ListCell<Message> call( ListView<Message> param ) {
      @SuppressWarnings("unused")
      final ListCell<Message> cell = new ListCell<>() {
         @Override
         protected void updateItem( Message item, boolean empty) {
            super.updateItem( item, empty );
            if( empty ) {
               setGraphic( null );
            }
            else {
               final HBox hBox = new HBox();
               hBox.setAlignment( item._alignment );
               final Label label = new Label( item._text );
               label.setAlignment( item._alignment );
               label.setTextFill( item._color );
               hBox.getChildren().add( label );
               setGraphic( hBox );
            }
         }
      };
      return cell;
   }
}
