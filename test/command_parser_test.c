#include <chat/command_parser.h>

#include "tests_utils.h"
#include "all_tests.h"

#include <string.h>

void command_parser_test( TST_context * ctxt ) {
   TST_chapter( ctxt, "REPL interface tests" );
   command_parser parser = command_parser_Zero;
   value          token  = value_Zero;

   strcpy( parser.line, "join,Le titi pâpâte,239.0.0.66,2416" );
   ASSERT( ctxt, command_parser_reset        ( &parser )                                  , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( ctxt, strcmp( "join", token.string ) == 0                                      , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( ctxt, strcmp( "Le titi pâpâte", token.string ) == 0                            , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( ctxt, strcmp( "239.0.0.66", token.string ) == 0                                , true );
   ASSERT( ctxt, command_parser_get_port     ( &parser, &token )                          , true );
   ASSERT( ctxt, token.ushort_value == 2416                                               , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , false );

   strcpy( parser.line, "debug,true" );
   ASSERT( ctxt, command_parser_reset        ( &parser )                                  , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( ctxt, strcmp( "debug", token.string ) == 0                                     , true );
   ASSERT( ctxt, command_parser_get_boolean  ( &parser, &token )                          , true );
   ASSERT( ctxt, token.boolean_value                                                      , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , false );

   strcpy( parser.line, "send,Salut à toi, le chevelu à la barbe hirsute" );
   ASSERT( ctxt, command_parser_reset        ( &parser )                                  , true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , true );
   ASSERT( ctxt, strcmp( "send", token.string ) == 0                                      , true );
   ASSERT( ctxt, command_parser_get_remaining( &parser, &token )                          , true );
   ASSERT( ctxt, strcmp( "Salut à toi, le chevelu à la barbe hirsute", token.string ) == 0, true );
   ASSERT( ctxt, command_parser_get_string   ( &parser, &token )                          , false );
}
