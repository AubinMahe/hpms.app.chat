#include "tests_utils.h"
#include "all_tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char * argv[] ) {
   bool no_user_arg       = ( argc == 1 );
   bool do_set_of_strings = no_user_arg;
   bool do_command_parser = no_user_arg;
   bool do_preferences    = no_user_arg;
   for( int i = 1; i < argc; ++i ) {
      const char * arg = argv[i];
      do_set_of_strings = false;
      do_command_parser = false;
      do_preferences    = false;
      if( 0 == strcmp( "set_of_strings", arg )) {
         do_set_of_strings = true;
      }
      else if( 0 == strcmp( "command_parser", arg )) {
         do_command_parser = true;
      }
      else if( 0 == strcmp( "preferences", arg )) {
         do_preferences = true;
      }
      else {
         fprintf( stderr, "Unexpected argument: %s\n", arg );
         return EXIT_FAILURE;
      }
   }
   TST_context ctxt = TST_context_Zero;
   if( do_set_of_strings ) {
      set_of_strings_test( &ctxt );
   }
   if( do_command_parser ) {
      command_parser_test( &ctxt );
   }
   if( do_preferences ) {
      preferences_test( &ctxt );
   }
   TST_ended( &ctxt );
   return EXIT_SUCCESS;
}
