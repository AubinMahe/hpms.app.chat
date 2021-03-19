#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"
#include "command_parser.h"

static void printer( const char * pseudo, const char * message ) {
   printf( "%s: %s\n", pseudo, message );
}

int main( void ) {
   command_parser parser;
   chat           cht  = NULL;
   arguments      args = { NONE };
   const char *   help = "";
   for( chat_new( &cht, printer ); ( args.command != CLOSE )&& command_parser_readline( &parser ); ) {
      help = "Available commands are: verbosity, join, send, leave, close";
      args.command = NONE;
      value v;
      if( ! command_parser_get_string( &parser, &v )) {
         continue;
      }
      if( 0 == strcmp( "verbosity", v.string )) {
         if( command_parser_get_string( &parser, &v )) {
            if( 0 ==  strcasecmp( v.string, "mute" )) {
               args.command = VERBOSITY;
               args.values[0].ushort_value = 0;
            }
            else if( 0 ==  strcasecmp( v.string, "info" )) {
               args.command = VERBOSITY;
               args.values[0].ushort_value = 1;
            }
            else if( 0 ==  strcasecmp( v.string, "details" )) {
               args.command = VERBOSITY;
               args.values[0].ushort_value = 2;
            }
            else {
               help = "Syntax: verbosity,<verbosity> in [MUTE, INFO, DETAILS]";
            }
         }
         else {
            help = "Syntax: verbosity,<verbosity> in [MUTE, INFO, DETAILS]";
         }
      }
      else if( 0 == strcmp( "join", v.string )) { // join,Aubin,239.0.0.66,2416 join,Eve,239.0.0.66,2416
         if(   command_parser_get_string( &parser, &args.values[0] )
            && command_parser_get_string( &parser, &args.values[1] )
            && command_parser_get_port( &parser, &args.values[2] ))
         {
            args.command = JOIN;
         }
         else {
            help = "Syntax: join,<pseudo>,<multicast address>,<port> with: multicast in 239.0.0.0/8, port > 1024";
         }
      }
      else if( 0 == strcmp( "send", v.string )) {
         if( command_parser_get_string( &parser, &args.values[0] )) {
            args.command = SEND;
         }
         else {
            help = "Syntax: send,<line of text>";
         }
      }
      else if( 0 == strcmp( "leave", v.string )) {
         args.command = LEAVE;
      }
      else if( 0 == strcmp( "close", v.string )) {
         args.command = CLOSE;
      }
      if( args.command == NONE ) {
         fprintf( stdout, "%s.\n", help );
      }
      else {
         bool ok = false;
         switch( args.command ) {
         case VERBOSITY: ok = chat_set_verbosity( cht, args.values[0].ushort_value ); break;
         case JOIN     : ok = chat_join( cht, args.values[0].string, args.values[1].string, args.values[1].ushort_value ); break;
         case SEND     : ok = chat_send( cht, args.values[0].string ); break;
         case LEAVE    : ok = chat_leave( cht ); break;
         case CLOSE    : ok = true; break;
         default: break;
         }
         if( ! ok ) {
            fprintf( stdout, "Command not successfuly executed\n" );
         }
      }
   }
   return chat_close( &cht ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
