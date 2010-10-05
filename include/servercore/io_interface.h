#ifndef HEADER_D49BD3FC37ABAF61
#define HEADER_D49BD3FC37ABAF61

/***************************************************************************
 *   Copyright (C) 2009 by nordin digivox   															 *
 *   nordin@digivox.nl   																									 *
 *                                                                         *
 ***************************************************************************/

#ifndef _IO_INTERFACE_H
#define _IO_INTERFACE_H

#include "linuxlist.h"
#include "io_process.h"

// iotype
#define	RX	0x01
#define TX	0x10
#define EX	0x11


int32_t send_infomsg(char* msg);

int32_t init_tcp_server(int32_t portnr, char ip,
                        void (*rcv_cb)(int32_t fd, char* data, int32_t len));

int32_t accept_new_client(struct fd_obj* fdo);

int32_t send_to_client(char* data, int32_t len);

int32_t connect_socket(unsigned char * ipaddr,
                       int32_t portnr, int32_t protocol);



#endif /* _IO_INTERFACE_H */

#endif // header guard
