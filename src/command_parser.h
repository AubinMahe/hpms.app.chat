#pragma once

#include <stdbool.h>

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

extern const value value_Zero;

typedef struct {
   command command;
   value   values[3];
} arguments;

extern const arguments arguments_Zero;

typedef struct {
   char   line[STRING_MAX];
   char * token_start;
   char * token_end;
} command_parser;

extern const command_parser command_parser_Zero;

bool command_parser_readline( command_parser * This );
bool command_parser_reset( command_parser * This );
bool command_parser_get_string( command_parser * This, value * dest );
bool command_parser_get_remaining( command_parser * This, value * dest );
bool command_parser_get_boolean( command_parser * This, value * dest );
bool command_parser_get_port( command_parser * This, value * dest );
