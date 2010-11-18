/***************************************************************************
 *   El Andaluz
 *
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
int32_t init_tcp_server(int32_t portnr, char* ip,
                        void (*rcv_cb)(int32_t fd, char* data, int32_t len));
int32_t accept_new_client(struct fd_obj* fdo);


#endif /* _IO_INTERFACE_H */

