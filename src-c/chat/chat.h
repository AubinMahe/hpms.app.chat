#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <utils/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * chat;

typedef unsigned char byte;

typedef enum {
   MUTE,
   INFO,
   DETAILS
} chat_verbosity;

typedef void (* chat_message_consumer)( const char * pseudo, const char * message );

DLL_PUBLIC bool chat_new          ( chat * This, chat_message_consumer consumer );
DLL_PUBLIC bool chat_set_verbosity( chat   This, chat_verbosity verbosity );
DLL_PUBLIC bool chat_join         ( chat   This, const char * pseudo, const char * mcast, unsigned short port );
DLL_PUBLIC bool chat_is_alive     ( chat   This );
DLL_PUBLIC bool chat_send         ( chat   This, const char * message );
DLL_PUBLIC bool chat_receive      ( chat   This, byte * payload, size_t * payload_size );
DLL_PUBLIC bool chat_leave        ( chat   This );
DLL_PUBLIC bool chat_close        ( chat * This );

#ifdef __cplusplus
}
#endif
