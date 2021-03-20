#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "utils_comparator.h"
#include "utils_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * utils_map;

typedef bool (* utils_map_iterator)( size_t index, const char * key, void * value, void * user_context );

DLL_PUBLIC bool utils_map_new    ( utils_map * map, utils_comparator comparator );
DLL_PUBLIC bool utils_map_put    ( utils_map map, void * key, void * value );
DLL_PUBLIC bool utils_map_get    ( utils_map map, const void * key, void ** value );
DLL_PUBLIC bool utils_map_foreach( utils_map map, utils_map_iterator iter, void * user_context );
DLL_PUBLIC bool utils_map_clear  ( utils_map * map, bool free_key_and_value );
DLL_PUBLIC bool utils_map_delete ( utils_map * map, bool free_key_and_value );

#ifdef __cplusplus
}
#endif
