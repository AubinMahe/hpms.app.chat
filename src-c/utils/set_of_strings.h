#pragma once

#include <stdbool.h>

typedef struct { unsigned unused; } * set_of_strings;

typedef bool ( * set_of_strings_iterator)( unsigned index, const char * item, void * user_context );

bool set_of_strings_new( set_of_strings * set );
bool set_of_strings_contains( set_of_strings set, const char * string );
bool set_of_strings_add( set_of_strings set, const char * string );
bool set_of_strings_remove( set_of_strings set, const char * string );
bool set_of_strings_replace( set_of_strings set, const char * old, const char * new );
bool set_of_strings_clear( set_of_strings set );
bool set_of_strings_size( set_of_strings set, unsigned * cardinality );
bool set_of_strings_foreach( set_of_strings set, set_of_strings_iterator iter, void * user_context );
bool set_of_strings_delete( set_of_strings * set );
