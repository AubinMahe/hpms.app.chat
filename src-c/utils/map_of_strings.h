#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <utils/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * map_of_strings;

typedef bool (* map_iterator)( size_t index, const char * key, void * value, void * user_context );

DLL_PUBLIC bool map_of_strings_new( map_of_strings * map );
DLL_PUBLIC bool map_of_strings_put( map_of_strings map, const char * key, void * value );
DLL_PUBLIC bool map_of_strings_get( map_of_strings map, const char * key, void ** value );
DLL_PUBLIC bool map_of_strings_foreach( map_of_strings map, map_iterator iter, void * user_context );
DLL_PUBLIC bool map_of_strings_delete( map_of_strings * map, bool free_value );

#ifdef __cplusplus
}
#endif
