#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct { unsigned unused; } * chat;

typedef unsigned char byte;

typedef enum {
   MUTE,
   INFO,
   DETAILS
} chat_verbosity;

typedef void (* chat_message_consumer)( const char * pseudo, const char * message );

bool chat_new          ( chat * This, chat_message_consumer consumer );
bool chat_set_verbosity( chat   This, chat_verbosity verbosity );
bool chat_join         ( chat   This, const char * pseudo, const char * mcast, unsigned short port );
bool chat_is_alive     ( chat   This );
bool chat_send         ( chat   This, const char * message );
bool chat_receive      ( chat   This, byte * payload, size_t * payload_size );
bool chat_leave        ( chat   This );
bool chat_close        ( chat * This );
