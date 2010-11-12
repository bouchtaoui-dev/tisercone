#ifndef SELECT_TIMER_H_INCLUDED
#define SELECT_TIMER_H_INCLUDED

#include <sys/time.h>
#include "linuxlist.h"

/* voorlopig is nauwkeurigheid alleen in seconden */
struct timer_caller {
    struct llist_head list;
    struct timeval tv;    //time-out in seconds
    int id;
    int heap;   //als het object uit de heap is gemaakt, dan geheugen opruimen.
    void (*cb_timer)(void);
};


void enqueue_timer_caller(struct timer_caller* tc);
struct timer_caller* dequeue_timer_caller(void);
void dispatch_timer_caller(struct timer_caller* tc);
void update_timer_list(void);


#endif // SELECT_TIMER_H_INCLUDED
