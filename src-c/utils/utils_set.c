#include "utils_set.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h> // memset

typedef struct {
   utils_comparator comparator;
   unsigned         count;
   const void **    data;
} utils_set_private;

bool utils_set_new( utils_set * set, utils_comparator comparator ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = malloc( sizeof( utils_set_private ));
   if( This == NULL ) {
      return false;
   }
   memset( This, 0, sizeof( utils_set_private ));
   This->comparator = comparator;
   *set = (void *)This;
   return true;
}

bool utils_set_contains( utils_set set, const void * string ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( This->data == NULL ) {
      return false;
   }
   void ** res = bsearch( &string, This->data, This->count, sizeof( void *), This->comparator );
   return res != NULL;
}

bool utils_set_add( utils_set set, const void * item ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   const void ** data = realloc( This->data, ( 1 + This->count ) * sizeof( const void * ));
   if( data == NULL ) {
      return false;
   }
   This->data = data;
   This->data[This->count] = item;
   ++This->count;
   qsort( This->data, This->count, sizeof( char *), This->comparator );
   return true;
}

bool utils_set_remove( utils_set set, const void * data, bool free_data ) {
   if(( set == NULL )||( data == NULL )) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( This->data == NULL ) {
      return false;
   }
   const void ** res = bsearch( &data, This->data, This->count, sizeof( const void *), This->comparator );
   if( res ) {
      if( free_data ) {
         // Astuce pour libérer un pointeur const void * en espérant qu'il ne soit pas "vraiment" const,
         // sinon c'est SIGSEGV !
         // Ne fonctionne que si les pointeurs sont stockables sur 64 bits
         free((void *)(uint64_t)*res );
      }
      ssize_t index = res - This->data;
      --This->count;
      memmove( res, res + 1, ( This->count - (size_t)index )*sizeof(char *));
      return true;
   }
   return false;
}

bool utils_set_replace( utils_set set, const void * old_data, const void * new_data, bool free_old_data ) {
   if(( set == NULL )||( old_data == NULL )) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( This->data == NULL ) {
      return false;
   }
   const void ** res = bsearch( &old_data, This->data, This->count, sizeof( const void *), This->comparator );
   if( res ) {
      if( free_old_data ) {
         // Astuce pour libérer un pointeur const void * en espérant qu'il ne soit pas "vraiment" const,
         // sinon c'est SIGSEGV !
         // Ne fonctionne que si les pointeurs sont stockables sur 64 bits
         free((void *)(int64_t)*res );
      }
      *res = new_data;
      qsort( This->data, This->count, sizeof( char *), This->comparator );
      return true;
   }
   return false;
}


bool utils_set_size( utils_set set, unsigned * cardinality ) {
   if(( set == NULL )||( cardinality == NULL )) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   *cardinality = This->count;
   return true;
}

bool utils_set_foreach( utils_set set, utils_set_iterator iter, void * user_context ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   for( unsigned i = 0; i < This->count; ++i ) {
      if( ! iter( i, This->data[i], user_context )) {
         return false;
      }
   }
   return true;
}

bool utils_set_clear( utils_set set, bool free_data ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( free_data ) {
      for( unsigned i = 0; i < This->count; ++i ) {
         const void * p = This->data[i];
         // Astuce pour libérer un pointeur const void * en espérant qu'il ne soit pas "vraiment" const,
         // sinon c'est SIGSEGV !
         // Ne fonctionne que si les pointeurs sont stockables sur 64 bits
         free((void *)(int64_t)p );
      }
   }
   free( This->data );
   This->data  = NULL;
   This->count = 0U;
   return true;
}

bool utils_set_delete( utils_set * pset, bool free_data ) {
   if( pset && utils_set_clear( *pset, free_data )) {
      free( *pset );
      *pset = NULL;
      return true;
   }
   return false;
}
