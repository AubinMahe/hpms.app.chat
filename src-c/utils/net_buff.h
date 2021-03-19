#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <netinet/in.h>

typedef unsigned char byte;

typedef struct { unsigned unused; } * net_buff;

bool net_buff_new( net_buff * net_buff, size_t capacity );
bool net_buff_get_capacity( net_buff net_buff, size_t * capacity );
bool net_buff_get_limit( net_buff net_buff, size_t * limit );
bool net_buff_get_position( net_buff net_buff, size_t * position );
bool net_buff_clear( net_buff net_buff );
bool net_buff_encode_string( net_buff net_buff, const char * src );
bool net_buff_flip( net_buff net_buff );
bool net_buff_send( net_buff net_buff, int sckt, struct sockaddr_in * to );
bool net_buff_receive( net_buff net_buff, int sckt, struct sockaddr_in * from );
bool net_buff_decode_string( net_buff net_buff, char * dest, size_t dest_size );
bool net_buff_delete( net_buff * net_buff );
