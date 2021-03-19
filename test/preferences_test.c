#include "preferences.h"
#include "tests_utils.h"
#include "all_tests.h"

#include <string.h>

void preferences_test( TST_context * ctxt ) {
   TST_chapter( ctxt, "Preferences" );
   preferences prefs = NULL;
   const char *   address = NULL;
   const char *   pseudo  = NULL;
   long           l_port  = 0;
   int            i_port  = 0;
   unsigned short us_port = 0;
   ASSERT( ctxt, preferences_load( &prefs, "chat" )                  , true );
   ASSERT( ctxt, preferences_get_string( prefs, "pseudo" , &pseudo  ), true );
   ASSERT( ctxt, strcmp( pseudo, "Le Titi-papatte" ) == 0            , true );
   ASSERT( ctxt, preferences_get_string( prefs, "address", &address ), true );
   ASSERT( ctxt, strcmp( address, "239.0.0.66" ) == 0                , true );
   ASSERT( ctxt, preferences_get_long  ( prefs, "port"   , &l_port  ), true );
   ASSERT( ctxt, l_port  == 2416L                                    , true );
   ASSERT( ctxt, preferences_get_int   ( prefs, "port"   , &i_port  ), true );
   ASSERT( ctxt, i_port  == 2416                                     , true );
   ASSERT( ctxt, preferences_get_ushort( prefs, "port"   , &us_port ), true );
   ASSERT( ctxt, us_port == 2416                                     , true );
   ASSERT( ctxt, preferences_delete( &prefs )                        , true );
}
