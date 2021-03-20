#include <chat/command_parser.h>

#include "all_tests.h"

#include <string.h>

void command_parser_test( struct tests_report * report ) {
   tests_chapter( report, "REPL interface tests" );
   command_parser parser = command_parser_Zero;
   value          token  = value_Zero;

   strcpy( parser.line, "join,Le titi pâpâte,239.0.0.66,2416" );
   ASSERT( report, command_parser_reset        ( &parser )                                  , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( report, strcmp( "join", token.string ) == 0                                      , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( report, strcmp( "Le titi pâpâte", token.string ) == 0                            , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( report, strcmp( "239.0.0.66", token.string ) == 0                                , true );
   ASSERT( report, command_parser_get_port     ( &parser, &token )                          , true );
   ASSERT( report, token.ushort_value == 2416                                               , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , false );

   strcpy( parser.line, "debug,true" );
   ASSERT( report, command_parser_reset        ( &parser )                                  , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( report, strcmp( "debug", token.string ) == 0                                     , true );
   ASSERT( report, command_parser_get_boolean  ( &parser, &token )                          , true );
   ASSERT( report, token.boolean_value                                                      , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , false );

   strcpy( parser.line, "send,Salut à toi, le chevelu à la barbe hirsute" );
   ASSERT( report, command_parser_reset        ( &parser )                                  , true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( report, strcmp( "send", token.string ) == 0                                      , true );
   ASSERT( report, command_parser_get_remaining( &parser, &token )                          , true );
   ASSERT( report, strcmp( "Salut à toi, le chevelu à la barbe hirsute", token.string ) == 0, true );
   ASSERT( report, command_parser_get_string   ( &parser, &token )                          , false );
}
