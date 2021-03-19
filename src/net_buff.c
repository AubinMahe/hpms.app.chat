#include "net_buff.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
   byte * buffer;
   size_t capacity;
   size_t limit;
   size_t position;
} net_buff_private;

bool net_buff_new( net_buff * nb, size_t capacity ) {
   if( nb == NULL ) {
      return false;
   }
   if( capacity == 0 ) {
      return false;
   }
   net_buff_private * This = malloc( sizeof( net_buff_private ));
   memset( This, 0, sizeof( net_buff_private ));
   This->buffer   = malloc( capacity );
   This->capacity = capacity;
   This->limit    = capacity;
   *nb = (net_buff)This;
   return true;
}

bool net_buff_get_capacity( net_buff nb, size_t * capacity ) {
   if( nb == NULL ) {
      return false;
   }
   if( capacity == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   *capacity = This->capacity;
   return true;
}

bool net_buff_get_limit( net_buff nb, size_t * limit ) {
   if( nb == NULL ) {
      return false;
   }
   if( limit == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   *limit = This->limit;
   return true;
}

bool net_buff_get_position( net_buff nb, size_t * position ) {
   if( nb == NULL ) {
      return false;
   }
   if( position == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   *position = This->position;
   return true;
}

bool net_buff_clear( net_buff nb ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   This->position = 0;
   This->limit    = This->capacity;
   return true;
}

bool net_buff_encode_string( net_buff nb, const char * string ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + 4 > This->limit ) {
      return false;
   }
   size_t   length         = strlen( string );
   uint32_t encoded_length = htonl((uint32_t)length );
   memcpy( This->buffer + This->position, &encoded_length, sizeof( encoded_length ));
   This->position += 4;
   if( This->position + length > This->limit ) {
      return false;
   }
   memcpy( This->buffer + This->position, string, length );
   This->position += length;
   return true;
}

bool net_buff_flip( net_buff nb ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   This->limit = This->position;
   This->position = 0;
   return true;
}

bool net_buff_send( net_buff nb, int sckt, struct sockaddr_in * to ) {
   if( nb == NULL ) {
      return false;
   }
   if( to == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   size_t  length = This->limit - This->position;
   ssize_t nbytes = sendto( sckt, This->buffer + This->position, length, 0, (struct sockaddr*)to, sizeof( struct sockaddr_in ));
   if( nbytes >= 0 ) {
      This->position += (size_t)nbytes;
      return ((size_t)nbytes) == length;
   }
   perror( "sendto" );
   return false;
}

bool net_buff_receive( net_buff nb, int sckt, struct sockaddr_in * from ) {
   if(( nb == NULL )||( from == NULL )) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      fprintf( stderr, "%s: You must call net_buff_wrap first\n", __func__ );
      return false;
   }
   socklen_t addrlen = sizeof( struct sockaddr_in );
   size_t    length  = This->limit - This->position;
   ssize_t   nbytes  = recvfrom( sckt, This->buffer + This->position, length, 0, (struct sockaddr *)from, &addrlen );
   if( nbytes >= 0 ) {
      This->position += (size_t)nbytes;
      return true;
   }
   perror( "recvfrom" );
   return false;
}

bool net_buff_decode_string( net_buff nb, char * target, size_t target_size ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + 4 > This->limit ) {
      return false;
   }
   uint32_t length = ntohl( *(uint32_t *)( This->buffer + This->position ));
   This->position += 4;
   if( length >= target_size ) {
      return false;
   }
   strncpy( target, (char *)( This->buffer + This->position ), length );
   target[length] = '\0';
   This->position += length;
   return true;
}

bool net_buff_delete( net_buff * nb ) {
   if( nb == NULL ) {
      return false;
   }
   if( *nb ) {
      net_buff_private * This = (net_buff_private *)*nb;
      free( This->buffer );
      free( This );
      *nb = NULL;
   }
   return true;
}
