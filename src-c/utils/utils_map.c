#include "utils_map.h"

#include <stdio.h>  // perror
#include <stdlib.h> // malloc, realloc, qsort, bsearch

typedef struct {
   void * key;
   void * value;
} pair;

typedef struct {
   utils_comparator comparator;
   size_t           count;
   pair *           data;
} utils_map_private;

bool utils_map_new( utils_map * map, utils_comparator comparator ) {
   if( map == NULL ) {
      return false;
   }
   utils_map_private * This = malloc( sizeof( utils_map_private ));
   if( This == NULL ) {
      perror( "malloc" );
      return false;
   }
   *map = (utils_map)This;
   This->comparator = comparator;
   This->count      = 0;
   This->data       = NULL;
   return true;
}

bool utils_map_put( utils_map map, void * key, void * value ) {
   if(( map == NULL )||( key == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   pair * data = realloc( This->data, ( This->count + 1 ) * sizeof( pair ));
   if( data == NULL ) {
      perror( "realloc" );
      return false;
   }
   This->data = data;
   This->data[This->count].key   = key;
   This->data[This->count].value = value;
   This->count++;
   qsort( This->data, This->count, sizeof( pair ), This->comparator );
   return true;
}

bool utils_map_get( utils_map map, const void * key, void ** value ) {
   if(( map == NULL )||( key == NULL )||( value == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   pair * p = bsearch( &key, This->data, This->count, sizeof( pair ), This->comparator );
   if( p == NULL ) {
      return false;
   }
   *value = p->value;
   return true;
}

bool utils_map_foreach( utils_map map, utils_map_iterator iter, void * user_context ) {
   if(( map == NULL )||( iter == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   for( size_t i = 0; i < This->count; ++i ) {
      if( ! (*iter)( i, This->data[i].key, This->data[i].value, user_context )) {
         break;
      }
   }
   return true;
}

bool utils_map_clear( utils_map * map, bool free_key_and_value ) {
   if( map == NULL ) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)*map;
   for( size_t i = 0; i < This->count; ++i ) {
      if( free_key_and_value ) {
         free( This->data[i].key );
         free( This->data[i].value );
      }
   }
   free( This->data );
   This->data  = NULL;
   This->count = 0;
   return true;
}

bool utils_map_delete( utils_map * map, bool free_key_and_value ) {
   if( utils_map_clear( map, free_key_and_value )) {
      utils_map_private * This = (utils_map_private *)*map;
      free( This->data );
      free( This );
      *map = NULL;
      return true;
   }
   return false;
}
