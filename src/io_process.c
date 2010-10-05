/***************************************************************************
 *   Copyright (C) 2009 by nordin digivox   							   *
 *   nordin@digivox.nl   												   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include <servercore/io_process.h>
#include <servercore/log_writer.h>


static LLIST_HEAD(io_fdobj);
static u_int32_t glbmaxfd = 0;


struct fd_obj* add_fdo(int32_t fd)
{
	int32_t fdflag;
	struct fd_obj* fdo = malloc(sizeof(struct fd_obj));
	if(fdo == 0)
	{
		DEBUG_MSG("Couldn't allocate memory.\n");
		return NULL;
	}

	fdo->fd = fd;

	fdflag = fcntl(fdo->fd, F_GETFL);
	if(fdflag < 0)
		return NULL;

	fdflag |= O_NONBLOCK;
	fdflag = fcntl(fdo->fd, F_SETFL, fdflag);
	if(fdflag < 0)
		return NULL;

	if(fdo->fd > glbmaxfd)
		glbmaxfd = fdo->fd;

	/* voeg fd-object in de lijst */
	llist_add_tail(&fdo->list, &io_fdobj);

	fdo->fh = NULL;
	fdo->th = NULL;

	return fdo;
}

void remove_fdo(int32_t fd)
{
	struct fd_obj* fdo;

	llist_for_each_entry(fdo, &io_fdobj, list)
	{
		if(fdo->fd == fd)
			break;
	}

	llist_del(&fdo->list);
	if(fdo) {
		free(fdo);
	}
}

//Dit is de kern v/d applicatie, waar de hele app op gebaseerd is
int32_t io_select()
{
	int32_t running = 1;
	int32_t timed_out = 1;
	struct timeval time_out;

	while(running)
	{
		struct fd_obj* fdo, *tmp;
		fd_set fsread;
		int32_t res = 0;

		/* indien geen timeout, dan de resterende tijd meenemen */
		if(timed_out) {
			time_out.tv_sec = 1;
			time_out.tv_usec = 0;
		}

		timed_out = 0;

		FD_ZERO(&fsread);
		llist_for_each_entry(fdo, &io_fdobj, list)
		{
			FD_SET(fdo->fd, &fsread);
		}

		res = select(glbmaxfd+1, &fsread, 0, 0, &time_out);
		if(res < 0)
			running = 0;
		else if(res) {
			llist_for_each_entry_safe(fdo, tmp, &io_fdobj, list)
			{
				if(FD_ISSET(fdo->fd, &fsread)) {
					if(fdo->fh(fdo) < 0) {
						close(fdo->fd);
						remove_fdo(fdo->fd);
					}
				}
			}
		} else {
			timed_out = 1;
			llist_for_each_entry(fdo, &io_fdobj, list)
			{
				if(fdo->th)
					fdo->th();
			}
		}
	}

	return 0;
}

