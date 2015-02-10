/*
  Copyright <2014> Husain Alshehhi
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "thread.H"

/* The scheduler is implemented using a linked list. Each item in the linked
 * list is a pointer to a thread */
  
class Scheduler {
        /* Thus struct represents each item in the queue. Each item has two
         * variables: (1) value: which points to the thread in the item (2)
         * next: which points to the next item in the queue.*/
        
        struct item {
                Thread *value;
                item *next;
        };

        /* Here we have a pointer the first item in the linked list, as well as
         * a pointer to the first item in the waiting list. Note that the
         * waiting list is not used in this implementation. */
        
        item *ready_list;
        item *waiting_list;
        int isempty_ready_list;
        
    public:

        Scheduler();
        /* Setup the scheduler. This sets up the ready queue, for example.  If
           the scheduler implements some sort of round-robin scheme, then the
           end_of_quantum handler is installed here as well. */

        virtual void yield();
        /* Called by the currently running thread in order to give up the CPU.
           The scheduler selects the next thread from the ready queue to load
           onto the CPU, and calls the dispatcher function defined in
           'threads.h' to do the context switch. */

        virtual void resume(Thread * _thread);
        /* Add the given thread to the ready queue of the scheduler. This is
           called for threads that were waiting for an event to happen, or that
           have to give up the CPU in response to a preemption. */

        virtual void add(Thread * _thread);
        /* Make the given thread runnable by the scheduler. This function is
           called typically after thread creation. Depending on the
           implementation, this may not entail more than simply adding the
           thread to the ready queue (see scheduler_resume). */

        virtual void terminate(Thread * _thread);
        /* Remove the given thread from the scheduler in preparation for
           destruction of the thread. */
};
	
	

#endif
