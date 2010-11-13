/**
 * Author: El Andaluz
 * Real name: Nordin el Bouchtaoui
 * Date: 10/11/2010
 * Description:
 * This timer construction works only with select(). You can create
 * a timer object with a certain time and add it to the timer list.
 * If the time value passed, it will call the callback function you
 * provided in the timer object.
 *
 * It has an accuracy in the microsecond range, but the accuracy is
 * not guaranteed. It depends on how fast your system is, how many
 * processes running on your system, etc...
 *
 * Example usage:
 * struct timer_caller* tc = malloc(sizeof(struct timer_caller));
 * if(tc) {
 *      tc->tv.tv_sec = 4;  //set seconds
 *      tc->tv.tv_usec = 0; //set microseconds
 *      tc->cb_timer = foo; //callback function to be called
 *      tc->heap = 1;   //tc is created from the heap (malloc),
                        //if you don't set, it results a memoryleak.
 *      enqueue_timer_caller(tc);   //add the timer object to the list,
                                    //and timer starts ticking ;-)
 * }
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <servercore/select_timer.h>
#include <servercore/log_writer.h>


static LLIST_HEAD(timer_qu);
static struct timeval cur_time = {0};
static struct timeval prev_time = {0};

void init_timer_select(void)
{
    if(!gettimeofday(&prev_time, NULL)) {
        memset(&prev_time, 0x00, sizeof(struct timeval));
        perror("gettimeofday");
    }
}

void update_timer_list(void)
{
    struct timer_caller* tc = NULL;
    struct timeval diff = {0};

    if(llist_empty(&timer_qu)) {
        goto reset;
    }

    if(gettimeofday(&cur_time, NULL)) {
        perror("gettimeofday");
        return;
    }

    timersub(&cur_time, &prev_time, &diff);

    llist_for_each_entry(tc, &timer_qu, list) {
        timersub(&tc->tv, &diff, &tc->tv);
        if(tc->tv.tv_sec < 0) {
            /* sometimes, diff is some microseconds bigger than the
             * shortest time, that's why we need to correct the remaining time
             * by setting it to zero.*/
            //memset(&tc->tv, 0x00, sizeof(struct timeval));
            tc->tv.tv_sec = 0;
            tc->tv.tv_usec = 0;
        }
    }

reset:
    if(gettimeofday(&prev_time, NULL)) {
        perror("gettimeofday");
        return;
    }
}


/**
 * enqueue_timer_caller - add timer_caller to the queue
 * @tc:     pointer to a timer_caller object to add to the queue
 *
 * Als timer_caller object toegevoegd wordt, dan wordt eerst gekeken
 * waar in de queue geplaatst wordt. Want de queue is geordend van de
 * kleinste time-out waarde naar de grootste time-out waarde. Elke
 * keer dat een timeout object uit de queue gehaald wordt, wordt dan
 * de kleinste waarde eruit gehaald.
 */
void enqueue_timer_caller(struct timer_caller* tc)
{
    int found = 0;
    struct timer_caller* temp = NULL;
    struct timer_caller* prev = NULL;

    if(llist_empty(&timer_qu)) {
        llist_add_tail(&tc->list, &timer_qu);
        return;
    }

    llist_for_each_entry(temp, &timer_qu, list) {
        if(timercmp(&tc->tv, &temp->tv, <=)) {
            if(prev)
                llist_add(&tc->list, &prev->list);
            else llist_add(&tc->list, &timer_qu);
            found = 1;
            break;
        }
        prev = temp;
    }

    if(!found) {
        llist_add_tail(&tc->list, &timer_qu);
    }
}

/**
 * dequeue_timer_caller - retrieve timer_caller object from the queue
 *
 * Elke keer dat een time-out object uit de queue gehaadld wordt,
 * moeten alle time-outs ge-updated worden, vergeet dat niet!
 * Let op! Het object wordt niet uit de queue verwijderd. Dat wordt
 * namelijk in dispatch_timer_caller gedaan.
 */
struct timer_caller* dequeue_timer_caller(void)
{

    struct timer_caller* tc;

    if(llist_empty(&timer_qu))
        return NULL;


    llist_for_each_entry(tc, &timer_qu, list) {
        break;
    }

    if(gettimeofday(&prev_time, NULL)) {
        perror("gettimeofday");
        return NULL;
    }

    return tc;
}

/**
 * dispatch_timer_caller - calls the callback and cleans up allocated memory
 * @tc:     pointer to a timer_caller object
 *
 * Hier wordt de callback functie aangeroepen die mee gegeven was.
 * Als timer_caller object uit de heap gemaakt is, dient dat stukje
 * geheugen opgeruimd te worden. Is dat niet het geval, dan wordt dat
 * met rust gelaten. Verder wordt het object uit de queue verwijderd.
 */
void dispatch_timer_caller(struct timer_caller* tc)
{
    llist_del(&tc->list);
    if(tc->cb_timer) {
        tc->cb_timer();
    }

    DEBUG_MSG("Dispatched timer_caller: id %d  timeout(sec): %ld  timeout(usec): %ld",
              tc->id, tc->tv.tv_sec, tc->tv.tv_usec);

    if(tc->heap) {
        free(tc);
        tc = NULL;
    }
}


