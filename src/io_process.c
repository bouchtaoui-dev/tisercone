/***************************************************************************
 *   Copyright (C) 2009 by nordin digivox   							   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include <servercore/io_process.h>
#include <servercore/select_timer.h>
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

/* This is the core the application is based on */
int32_t io_select()
{
	int32_t running = 1;
	int32_t res = 0;
	struct fd_obj* fdo, *tmp;
	struct timeval tv;
	struct timer_caller* tc = NULL;
	fd_set fsread;

    timerclear(&tv);
	while(running)
	{
        tc = dequeue_timer_caller();
        if(tc) {
            memcpy(&tv, &tc->tv, sizeof(struct timeval));
            DEBUG_MSG("dequeued tc, id: %d  timeout: %ld", tc->id,
             tc->tv.tv_sec);
        } else timerclear(&tv);

		FD_ZERO(&fsread);
		llist_for_each_entry(fdo, &io_fdobj, list)
		{
			FD_SET(fdo->fd, &fsread);
		}

		res = select(glbmaxfd+1, &fsread, 0, 0, timerisset(&tv)?&tv:NULL);
		if(res < 0) {
			running = 0;
			continue;
		}

		update_timer_list();
		if(res) {
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
			if(tc)
                dispatch_timer_caller(tc);
		}
		update_timer_list();
	}

	return 0;
}

