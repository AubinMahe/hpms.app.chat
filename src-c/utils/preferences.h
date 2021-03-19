#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <utils/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * preferences;

DLL_PUBLIC bool preferences_load( preferences * prefs, const char * program_name );
DLL_PUBLIC bool preferences_get_path( preferences prefs, const char ** dest );
DLL_PUBLIC bool preferences_get_boolean( preferences prefs, const char * name, bool * value );
DLL_PUBLIC bool preferences_get_ushort( preferences prefs, const char * name, unsigned short * value );
DLL_PUBLIC bool preferences_get_int( preferences prefs, const char * name, int * value );
DLL_PUBLIC bool preferences_get_long( preferences prefs, const char * name, long * value );
DLL_PUBLIC bool preferences_get_string( preferences prefs, const char * name, const char ** value );
DLL_PUBLIC bool preferences_delete( preferences * prefs );

#ifdef __cplusplus
}
#endif
