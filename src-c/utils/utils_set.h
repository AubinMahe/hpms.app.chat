#pragma once

#include <stdbool.h>

#include "utils_comparator.h"
#include "utils_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * utils_set;

typedef bool ( * utils_set_iterator)( unsigned index, const void * data, void * user_context );
typedef bool ( * utils_set_comparator)( const void * left, const void * right );

DLL_PUBLIC bool utils_set_new     ( utils_set * set, utils_comparator comparator );
DLL_PUBLIC bool utils_set_add     ( utils_set   set, const void * data );
DLL_PUBLIC bool utils_set_remove  ( utils_set   set, const void * data, bool free_data );
DLL_PUBLIC bool utils_set_replace ( utils_set   set, const void * old_data, const void * new_data, bool free_old_data );
DLL_PUBLIC bool utils_set_contains( utils_set   set, const void * data );
DLL_PUBLIC bool utils_set_size    ( utils_set   set, unsigned * cardinality );
DLL_PUBLIC bool utils_set_foreach ( utils_set   set, utils_set_iterator iter, void * user_context );
DLL_PUBLIC bool utils_set_clear   ( utils_set   set, bool free_data );
DLL_PUBLIC bool utils_set_delete  ( utils_set * set, bool free_data );

#ifdef __cplusplus
}
#endif
