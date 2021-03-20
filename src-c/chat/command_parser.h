#pragma once

#include <stdbool.h>

#include <utils/utils_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_MAX      (64*1024)

typedef enum {
   NONE,
   VERBOSITY,
   JOIN,
   SEND,
   LEAVE,
   CLOSE,
} command;

typedef union {
   char           string[STRING_MAX];
   unsigned short ushort_value;
   bool           boolean_value;
} value;

extern DLL_PUBLIC const value value_Zero;

typedef struct {
   command command;
   value   values[3];
} arguments;

extern DLL_PUBLIC const arguments arguments_Zero;

typedef struct {
   char   line[STRING_MAX];
   char * token_start;
   char * token_end;
} command_parser;

extern DLL_PUBLIC const command_parser command_parser_Zero;

DLL_PUBLIC bool command_parser_readline( command_parser * This );
DLL_PUBLIC bool command_parser_reset( command_parser * This );
DLL_PUBLIC bool command_parser_get_string( command_parser * This, value * dest );
DLL_PUBLIC bool command_parser_get_remaining( command_parser * This, value * dest );
DLL_PUBLIC bool command_parser_get_boolean( command_parser * This, value * dest );
DLL_PUBLIC bool command_parser_get_port( command_parser * This, value * dest );

#ifdef __cplusplus
}
#endif
