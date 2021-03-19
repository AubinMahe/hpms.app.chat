#include "set_of_strings.h"

#include <stdlib.h>
#include <string.h>

typedef struct {

   unsigned count;
   char **  strings;

} set_of_strings_private;

static int stringcmp( const void * vl, const void * vr ) {
   const char * const * al = vl;
   const char *         l  = *al;
   const char * const * ar  = vr;
   const char *         r  = *ar;
   return strcmp( l, r );
}

bool set_of_strings_new( set_of_strings * ss ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = malloc( sizeof( set_of_strings_private ));
   if( set == NULL ) {
      return false;
   }
   memset( set, 0, sizeof( set_of_strings_private ));
   *ss = (void *)set;
   return true;
}

bool set_of_strings_contains( set_of_strings ss, const char * string ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   if( set->strings == NULL ) {
      return false;
   }
   char ** res = bsearch( &string, set->strings, set->count, sizeof( char *), stringcmp );
   return res != NULL;
}

bool set_of_strings_add( set_of_strings ss, const char * string ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   char * new_string = strdup( string );
   if( new_string == NULL ) {
      return false;
   }
   char ** new_block = realloc( set->strings, (1+set->count) * sizeof( char *));
   if( new_block == NULL ) {
      free( new_string );
      return false;
   }
   set->strings = new_block;
   set->strings[set->count] = new_string;
   ++set->count;
   qsort( set->strings, set->count, sizeof( char *), stringcmp );
   return true;
}

bool set_of_strings_remove( set_of_strings ss, const char * string ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   if( set->strings == NULL ) {
      return false;
   }
   char ** res = bsearch( &string, set->strings, set->count, sizeof( char *), stringcmp );
   if( res ) {
      ssize_t index = res - set->strings;
      free( *res );
      --set->count;
      memmove( res, res + 1, ( set->count - (size_t)index )*sizeof(char *));
      return true;
   }
   return false;
}

bool set_of_strings_replace( set_of_strings ss, const char * old_value, const char * new_value ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   if( set->strings == NULL ) {
      return false;
   }
   char ** res = bsearch( &old_value, set->strings, set->count, sizeof( char *), stringcmp );
   if( res ) {
      free( *res );
      *res = strdup( new_value );
      qsort( set->strings, set->count, sizeof( char *), stringcmp );
      return true;
   }
   return false;
}


bool set_of_strings_size( set_of_strings ss, unsigned * cardinality ) {
   if(( ss == NULL )||( cardinality == NULL )) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   *cardinality = set->count;
   return true;
}

bool set_of_strings_foreach( set_of_strings ss, set_of_strings_iterator iter, void * user_context ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   for( unsigned i = 0; i < set->count; ++i ) {
      if( ! iter( i, set->strings[i], user_context )) {
         return false;
      }
   }
   return true;
}

bool set_of_strings_clear( set_of_strings ss ) {
   if( ss == NULL ) {
      return false;
   }
   set_of_strings_private * set = (set_of_strings_private *)ss;
   for( unsigned i = 0; i < set->count; ++i ) {
      free( set->strings[i] );
   }
   free( set->strings );
   set->strings = NULL;
   set->count   = 0U;
   return true;
}

bool set_of_strings_delete( set_of_strings * ss ) {
   if( ss == NULL ) {
      return false;
   }
   if( set_of_strings_clear( *ss )) {
      free( *ss );
      *ss = NULL;
      return true;
   }
   return false;
}
