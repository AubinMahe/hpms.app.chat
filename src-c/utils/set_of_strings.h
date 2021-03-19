#pragma once

#include <stdbool.h>

#include <utils/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * set_of_strings;

typedef bool ( * set_of_strings_iterator)( unsigned index, const char * item, void * user_context );

DLL_PUBLIC bool set_of_strings_new( set_of_strings * set );
DLL_PUBLIC bool set_of_strings_contains( set_of_strings set, const char * string );
DLL_PUBLIC bool set_of_strings_add( set_of_strings set, const char * string );
DLL_PUBLIC bool set_of_strings_remove( set_of_strings set, const char * string );
DLL_PUBLIC bool set_of_strings_replace( set_of_strings set, const char * old, const char * new );
DLL_PUBLIC bool set_of_strings_clear( set_of_strings set );
DLL_PUBLIC bool set_of_strings_size( set_of_strings set, unsigned * cardinality );
DLL_PUBLIC bool set_of_strings_foreach( set_of_strings set, set_of_strings_iterator iter, void * user_context );
DLL_PUBLIC bool set_of_strings_delete( set_of_strings * set );

#ifdef __cplusplus
}
#endif
