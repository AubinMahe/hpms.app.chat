#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct { unsigned unused; } * preferences;

bool preferences_load( preferences * prefs, const char * program_name );
bool preferences_get_path( preferences prefs, const char ** dest );
bool preferences_get_boolean( preferences prefs, const char * name, bool * value );
bool preferences_get_ushort( preferences prefs, const char * name, unsigned short * value );
bool preferences_get_int( preferences prefs, const char * name, int * value );
bool preferences_get_long( preferences prefs, const char * name, long * value );
bool preferences_get_string( preferences prefs, const char * name, const char ** value );
bool preferences_delete( preferences * prefs );
