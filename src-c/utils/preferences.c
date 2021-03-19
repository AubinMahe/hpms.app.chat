#include "preferences.h"
#include "map_of_strings.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
   char           rcpath[PATH_MAX];
   map_of_strings map;
} preferences_private;


bool preferences_load( preferences * prefs, const char * program_name ) {
   if(( prefs == NULL )||( program_name == NULL )) {
      return false;
   }
   preferences_private * This = malloc( sizeof( preferences_private ));
   if( This == NULL ) {
      perror( "malloc" );
      return false;
   }
   *prefs = (preferences)This;
   snprintf( This->rcpath, PATH_MAX, ".%src", program_name );
   FILE * chatrc = fopen( This->rcpath, "rt" );
   if( chatrc == NULL ) {
      const char * user_home = getenv( "HOME" );
      if( user_home ) {
         snprintf( This->rcpath, PATH_MAX, "%s/%s", user_home, program_name );
         chatrc = fopen( This->rcpath, "rt" );
         if( chatrc == NULL ) {
            snprintf( This->rcpath, PATH_MAX, "/etc/%s/%src", program_name, program_name );
            chatrc = fopen( This->rcpath, "rt" );
         }
      }
      else {
         snprintf( This->rcpath, PATH_MAX, "/etc/%s/%src", program_name, program_name );
         chatrc = fopen( This->rcpath, "rt" );
      }
   }
   if( chatrc == NULL ) {
      return false;
   }
   if( ! map_of_strings_new( &This->map )) {
      return false;
   }
   char buffer[2000];
   unsigned line = 0;
   while( fgets( buffer, sizeof( buffer ), chatrc )) {
      ++line;
      char * prop_name = buffer;
      while( isspace( *prop_name )) {
         ++prop_name;
      }
      if( *prop_name == '#' ) {
         continue;
      }
      char *  equal  = strchr( prop_name, '=' );
      char *  colon  = strchr( prop_name, ':' );
      char *  sep    = NULL;
      if( equal == NULL ) {
         sep = colon;
      }
      else if( colon == NULL ) {
         sep = equal;
      }
      else {
         sep = ( equal < colon ) ? equal : colon;
      }
      if( sep - prop_name < 0 ) {
         fprintf( stderr, "%s: preferences file syntax error at line %d\n", This->rcpath, line );
         return false;
      }
      *sep = '\0';
      prop_name = strtok( prop_name, " \t" );
      while( isspace( *++sep ));
      char * end = strrchr( sep, '\n' );
      if( end ) {
         while( isspace( *--end ));
         *++end = '\0';
      }
      map_of_strings_put( This->map, prop_name, strdup( sep ));
   }
   fclose( chatrc );
   return true;
}

bool preferences_get_path( preferences prefs, const char ** dest ) {
   if(( prefs == NULL )||( dest == NULL )) {
      return false;
   }
   preferences_private * This = (preferences_private *)prefs;
   *dest = This->rcpath;
   return true;
}

bool preferences_get_boolean( preferences prefs, const char * name, bool * value ) {
   const char * s = NULL;
   if( preferences_get_string( prefs, name, &s )) {
      if( 0 == strcmp( "true", s )) {
         *value = true;
         return true;
      }
      if( 0 == strcmp( "false", s )) {
         *value = false;
         return true;
      }
   }
   return false;
}

bool preferences_get_ushort( preferences prefs, const char * name, unsigned short * value ) {
   long lg = 0;
   if( preferences_get_long( prefs, name, &lg )) {
      *value = (unsigned short)lg;
      return true;
   }
   return false;
}

bool preferences_get_int( preferences prefs, const char * name, int * value ) {
   long lg = 0;
   if( preferences_get_long( prefs, name, &lg )) {
      *value = (int)lg;
      return true;
   }
   return false;
}

bool preferences_get_long( preferences prefs, const char * name, long * value ) {
   const char * s = NULL;
   if( ! preferences_get_string( prefs, name, &s )) {
      return false;
   }
   int base  = 10;
   if(( s[0] == '0' )&&(( s[1] == 'x' )||( s[1] == 'X' ))) {
      base = 16;
      s += 2;
   }
   char * err = NULL;
   *value = strtol( s, &err, base );
   return err &&( *err == '\0' );
}

bool preferences_get_string( preferences prefs, const char * name, const char ** value ) {
   if(( prefs == NULL )||( name == NULL )||( value == NULL )) {
      return false;
   }
   preferences_private * This = (preferences_private *)prefs;
   void * result = NULL;
   map_of_strings_get( This->map, name, &result );
   *value = result;
   return true;
}

bool preferences_delete( preferences * prefs ) {
   if( prefs == NULL ) {
      return false;
   }
   preferences_private * This = (preferences_private *)*prefs;
   map_of_strings_delete( &This->map, true );
   free( This );
   *prefs = NULL;
   return true;
}
