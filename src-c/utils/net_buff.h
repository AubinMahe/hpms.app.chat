#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <netinet/in.h>

#include <utils/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char byte;

typedef struct { unsigned unused; } * net_buff;

DLL_PUBLIC bool net_buff_new( net_buff * net_buff, size_t capacity );
DLL_PUBLIC bool net_buff_get_capacity( net_buff net_buff, size_t * capacity );
DLL_PUBLIC bool net_buff_get_limit( net_buff net_buff, size_t * limit );
DLL_PUBLIC bool net_buff_get_position( net_buff net_buff, size_t * position );
DLL_PUBLIC bool net_buff_clear( net_buff net_buff );
DLL_PUBLIC bool net_buff_encode_string( net_buff net_buff, const char * src );
DLL_PUBLIC bool net_buff_flip( net_buff net_buff );
DLL_PUBLIC bool net_buff_send( net_buff net_buff, int sckt, struct sockaddr_in * to );
DLL_PUBLIC bool net_buff_receive( net_buff net_buff, int sckt, struct sockaddr_in * from );
DLL_PUBLIC bool net_buff_decode_string( net_buff net_buff, char * dest, size_t dest_size );
DLL_PUBLIC bool net_buff_delete( net_buff * net_buff );

#ifdef __cplusplus
}
#endif
