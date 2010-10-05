#ifndef HEADER_22ED4A0273107176
#define HEADER_22ED4A0273107176

/***************************************************************************
 *   Copyright (C) 2009 by nordin digivox   															 *
 *   nordin@digivox.nl   																									 *
 *                                                                         *
 ***************************************************************************/

#ifndef _IO_PROCESS_H
#define _IO_PROCESS_H

#include "linuxlist.h"



struct fd_obj {
	struct llist_head list;
	int32_t fd;
	u_int32_t iotype;	// of het om ontvangen of verzenden gaat
	int32_t (*fh)(struct fd_obj* fdo);
	void (*th)(void);	//th = timer handler
	struct sockaddr* psockaddr;
	void* data;
};

int32_t io_select();
struct fd_obj* add_fdo(int32_t fd);
void remove_fdo(int32_t fd);


#endif //_IO_PROCESS_H

#endif // header guard 
