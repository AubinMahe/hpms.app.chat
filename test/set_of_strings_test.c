#include <utils/utils_set.h>

#include "tests_utils.h"
#include "all_tests.h"

#include <string.h>

static bool for_each( unsigned index, const void * item, void * user_context ) {
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

static int person_compare( const void * l, const void * r ) {
   const char * left  = *(const char * const *)l;
   const char * right = *(const char * const *)r;
   return strcmp( left, right );
}

void utils_set_test( TST_context * ctxt ) {
   TST_chapter( ctxt, "Sorted set of strings" );
   unsigned card = 0;
   utils_set set;
   ASSERT( ctxt, utils_set_new     ( &set, person_compare ) , true  );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 0                                  , true  );
   ASSERT( ctxt, utils_set_contains( set, "Aubin" )         , false );
   ASSERT( ctxt, utils_set_add     ( set, "Muriel" )        , true  );
   ASSERT( ctxt, utils_set_add     ( set, "Eve" )           , true  );
   ASSERT( ctxt, utils_set_add     ( set, "Aubin" )         , true  );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 3                                  , true  );
   ASSERT( ctxt, utils_set_contains( set, "Aubin" )         , true  );
   ASSERT( ctxt, utils_set_contains( set, "Muriel" )        , true  );
   ASSERT( ctxt, utils_set_contains( set, "Eve" )           , true  );
   ASSERT( ctxt, utils_set_contains( set, "Toto" )          , false );
   ASSERT( ctxt, utils_set_remove  ( set, "Eve", false )     , true  );
   ASSERT( ctxt, utils_set_contains( set, "Eve" )           , false );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 2                                  , true  );
   ASSERT( ctxt, utils_set_add     ( set, "Eve" )           , true  );
   ASSERT( ctxt, utils_set_contains( set, "Eve" )           , true  );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 3                                  , true  );
   ASSERT( ctxt, utils_set_foreach ( set, for_each, ctxt )  , true  );
   ASSERT( ctxt, utils_set_replace ( set, "Aubin", "Albert", false ), true  );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 3                                  , true  );
   ASSERT( ctxt, utils_set_contains( set, "Aubin" )         , false );
   ASSERT( ctxt, utils_set_contains( set, "Albert" )        , true  );
   ASSERT( ctxt, utils_set_remove  ( set, "Toto", false )           , false );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 3                                  , true  );
   ASSERT( ctxt, utils_set_remove  ( set, "Albert", false )         , true  );
   ASSERT( ctxt, utils_set_remove  ( set, "Muriel", false )         , true  );
   ASSERT( ctxt, utils_set_clear   ( set, false )                  , true  );
   ASSERT( ctxt, utils_set_contains( set, "Aubin" )         , false );
   ASSERT( ctxt, utils_set_contains( set, "Muriel" )        , false );
   ASSERT( ctxt, utils_set_contains( set, "Eve" )           , false );
   ASSERT( ctxt, utils_set_contains( set, "Toto" )          , false );
   ASSERT( ctxt, utils_set_size    ( set, &card )           , true  );
   ASSERT( ctxt, card == 0                                  , true  );
   ASSERT( ctxt, utils_set_delete  ( &set, false )                 , true  );
}
