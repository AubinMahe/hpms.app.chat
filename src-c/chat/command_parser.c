#include "command_parser.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const value          value_Zero          = { false };
const arguments      arguments_Zero      = { NONE, { value_Zero, value_Zero, value_Zero }};
const command_parser command_parser_Zero = { "", NULL, NULL };

#define PORT_MIN 1024

bool command_parser_readline( command_parser * This ) {
   if( This == NULL ) {
      return false;
   }
   memset( This->line, 0, sizeof( This->line ));
   if( fgets( This->line, STRING_MAX, stdin ) != NULL ) {
      return command_parser_reset( This );
   }
   perror( "fgets" );
   return false;
}

static void command_parser_search_end_of_token( command_parser * This ) {
   This->token_end   = strchr( This->token_start, ',' );
   if( This->token_end == NULL ) {
      This->token_end = strchr( This->token_start, '\n' );
      if( This->token_end == NULL ) {
         This->token_end = strchr( This->token_start, '\0' );
      }
   }
}

bool command_parser_reset( command_parser * This ) {
   if( This == NULL ) {
      return false;
   }
   This->token_start = This->line;
   command_parser_search_end_of_token( This );
   return true;
}

bool command_parser_get_string( command_parser * This, value * dest ) {
   if(( This == NULL )||( dest == NULL )) {
      return false;
   }
   if( This->token_end != NULL ) {
      ssize_t length = This->token_end - This->token_start;
      if( length > 0 ) {
         strncpy( dest->string, This->token_start, (size_t)length );
         dest->string[length] = '\0';
         if( *This->token_end ) {
            This->token_start = This->token_end + 1;
            command_parser_search_end_of_token( This );
         }
         else {
            This->token_start = This->token_end;
         }
         return true;
      }
   }
   return false;
}

bool command_parser_get_remaining( command_parser * This, value * dest ) {
   if(( This == NULL )||( dest == NULL )) {
      return false;
   }
   strcpy( dest->string, This->token_start );
   This->token_start = This->line + strlen( This->line );
   This->token_end   = This->token_start;
   return true;
}

bool command_parser_get_boolean( command_parser * This, value * dest ) {
   if(( This == NULL )||( dest == NULL )) {
      return false;
   }
   if( This->token_start ) {
      if( command_parser_get_string( This, dest )) {
         if( 0 == strcmp( dest->string, "true" )) {
            dest->boolean_value = true;
            return true;
         }
         if( 0 == strcmp( dest->string, "false" )) {
            dest->boolean_value = false;
            return true;
         }
      }
   }
   return false;
}

bool command_parser_get_port( command_parser * This, value * dest ) {
   if(( This == NULL )||( dest == NULL )) {
      return false;
   }
   if( This->token_start ) {
      if( command_parser_get_string( This, dest )) {
         char *   err   = NULL;
         long int iPort = strtol( dest->string, &err, 10 );
         if( err && ( *err == '\0' )) {
            if(( iPort >= PORT_MIN )&&( iPort <= USHRT_MAX )) {
               dest->ushort_value = (unsigned short)iPort;
               return true;
            }
         }
      }
   }
   return false;
}
