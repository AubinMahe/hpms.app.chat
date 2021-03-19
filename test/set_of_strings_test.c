#include "set_of_strings.h"
#include "tests_utils.h"
#include "all_tests.h"

#include <string.h>

static bool for_each( unsigned index, const char * item, void * user_context ) {
   TST_context * ctxt = (TST_context *)user_context;
   const bool unexpected_item = true;
   switch( index ) {
   case 0 : ASSERT( ctxt, strcmp( item, "Aubin"  ), 0 ); return true;
   case 1 : ASSERT( ctxt, strcmp( item, "Eve"    ), 0 ); return true;
   case 2 : ASSERT( ctxt, strcmp( item, "Muriel" ), 0 ); return true;
   default: ASSERT( ctxt, unexpected_item, false ); break;
   }
   return false;
}

void set_of_strings_test( TST_context * ctxt ) {
   TST_chapter( ctxt, "Sorted set of strings" );
   unsigned       card = 0;
   set_of_strings set;
   ASSERT( ctxt, set_of_strings_new     ( &set )                  , true  );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 0                                        , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Aubin" )          , false );
   ASSERT( ctxt, set_of_strings_add     ( set, "Muriel" )         , true  );
   ASSERT( ctxt, set_of_strings_add     ( set, "Eve" )            , true  );
   ASSERT( ctxt, set_of_strings_add     ( set, "Aubin" )          , true  );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 3                                        , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Aubin" )          , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Muriel" )         , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Eve" )            , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Toto" )           , false );
   ASSERT( ctxt, set_of_strings_remove  ( set, "Eve" )            , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Eve" )            , false );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 2                                        , true  );
   ASSERT( ctxt, set_of_strings_add     ( set, "Eve" )            , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Eve" )            , true  );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 3                                        , true  );
   ASSERT( ctxt, set_of_strings_foreach ( set, for_each, ctxt )   , true  );
   ASSERT( ctxt, set_of_strings_replace ( set, "Aubin", "Albert" ), true  );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 3                                        , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Aubin" )          , false );
   ASSERT( ctxt, set_of_strings_contains( set, "Albert" )         , true  );
   ASSERT( ctxt, set_of_strings_remove  ( set, "Toto" )           , false );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 3                                        , true  );
   ASSERT( ctxt, set_of_strings_remove  ( set, "Albert" )         , true  );
   ASSERT( ctxt, set_of_strings_remove  ( set, "Muriel" )         , true  );
   ASSERT( ctxt, set_of_strings_clear   ( set )                   , true  );
   ASSERT( ctxt, set_of_strings_contains( set, "Aubin" )          , false );
   ASSERT( ctxt, set_of_strings_contains( set, "Muriel" )         , false );
   ASSERT( ctxt, set_of_strings_contains( set, "Eve" )            , false );
   ASSERT( ctxt, set_of_strings_contains( set, "Toto" )           , false );
   ASSERT( ctxt, set_of_strings_size    ( set, &card )            , true  );
   ASSERT( ctxt, card == 0                                        , true  );
   ASSERT( ctxt, set_of_strings_delete  ( &set )                  , true  );
}
