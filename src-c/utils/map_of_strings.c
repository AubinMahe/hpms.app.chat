#include "map_of_strings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
   char * key;
   void * value;
} pair;

static int pair_compare( const void * l, const void * r ) {
   const pair * pl = (const pair *)l;
   const pair * pr = (const pair *)r;
   return strcmp( pl->key, pr->key );
}

typedef struct {
   size_t count;
   pair * data;
} map_of_strings_private;

bool map_of_strings_new( map_of_strings * map ) {
   if( map == NULL ) {
      return false;
   }
   map_of_strings_private * This = malloc( sizeof( map_of_strings_private ));
   if( This == NULL ) {
      perror( "malloc" );
      return false;
   }
   *map = (map_of_strings)This;
   This->count = 0;
   This->data  = NULL;
   return true;
}

bool map_of_strings_put( map_of_strings map, const char * key, void * value ) {
   if(( map == NULL )||( key == NULL )) {
      return false;
   }
   map_of_strings_private * This = (map_of_strings_private *)map;
   pair * data = realloc( This->data, ( This->count + 1 ) * sizeof( pair ));
   if( data == NULL ) {
      perror( "realloc" );
      return false;
   }
   This->data = data;
   This->data[This->count].key   = strdup( key );
   This->data[This->count].value = value;
   This->count++;
   qsort( This->data, This->count, sizeof( pair ), pair_compare );
   return true;
}

bool map_of_strings_get( map_of_strings map, const char * key, void ** value ) {
   if(( map == NULL )||( key == NULL )||( value == NULL )) {
      return false;
   }
   map_of_strings_private * This = (map_of_strings_private *)map;
   pair * p = bsearch( &key, This->data, This->count, sizeof( pair ), pair_compare );
   if( p == NULL ) {
      return false;
   }
   *value = p->value;
   return true;
}

bool map_of_strings_foreach( map_of_strings map, map_iterator iter, void * user_context ) {
   if(( map == NULL )||( iter == NULL )) {
      return false;
   }
   map_of_strings_private * This = (map_of_strings_private *)map;
   for( size_t i = 0; i < This->count; ++i ) {
      if( ! (*iter)( i, This->data[i].key, This->data[i].value, user_context )) {
         break;
      }
   }
   return true;
}

bool map_of_strings_delete( map_of_strings * map, bool free_value ) {
   if( map == NULL ) {
      return false;
   }
   map_of_strings_private * This = (map_of_strings_private *)*map;
   for( size_t i = 0; i < This->count; ++i ) {
      free( This->data[i].key );
      if( free_value ) {
         free( This->data[i].value );
      }
   }
   free( This->data );
   free( This );
   *map = NULL;
   return true;
}
