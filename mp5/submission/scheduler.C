// Copyright <2014> Husain Alshehhi

#include "scheduler.H"
#include "console.H"

typedef unsigned int size_t;

extern void* operator new(size_t);
extern void* operator new[](size_t);
extern void operator delete(void*);
extern void operator delete[](void*);

extern Scheduler* SYSTEM_SCHEDULER;
extern Thread* current_thread;

Scheduler::Scheduler() {
        ready_list -> value   = 0;
        ready_list -> next    = 0;
        waiting_list -> value = 0;
        waiting_list -> next  = 0;
}

/* Find the first item in the linked list and run it. The first item then is
 * deleted since there is no need to keep it in the linked list. Note that the
 * function does not save the thread since it is assumed that the thread
 * terminates when it yeilds */

void Scheduler::yield(void) {
        /* if (isempty_ready_list == 1) {
                Console::puts("EEERRROOORRR\n");
        } else {
                item* first_item = ready_list;
                if (ready_list -> next != 0)
                        ready_list = ready_list -> next;
                else
                        isempty_ready_list = 1;
                Thread::dispatch_to(first_item->value);
                }*/
        
        item* first_item = ready_list;
        if (first_item -> value != 0) {
                if (ready_list -> next != 0)
                        ready_list = ready_list -> next;
                Thread::dispatch_to(first_item->value);
        } else {
                Console::puts("EEEEEEEEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRROOOOOOOOOOOOOOOOOORRRRRRRRRRRRRRR\n");
        }
}

/* This adds the thread to the list of threads we have.  First, we allocate an
 * item for the thread. Second, we find the last entry in the linked
 * list. Thirdly, we append it after the last entry in the linked list.
 */

void Scheduler::add(Thread *_thread) {
        item *new_item   = new item;
        new_item -> value = _thread;
        new_item -> next  = 0;

        /* If this is the first time to add a thread, put it first. Otherwise,
         * put it at the end. */
        if (ready_list -> value == 0)
                ready_list = new_item;
        else {
                item *tmp = ready_list;
                while (tmp -> next != 0)
                        tmp = tmp -> next;
                tmp->next = new_item;
        }
}

/* Right now, there is no difference between resume and add. I might change that
 * in the future if I needed. */

void Scheduler::resume(Thread *_thread) {
        add(_thread);
}

/* There is no reason to implement terminate for the following reasons: (1), the
 * Scheduler does not keep track or save any information about the running
 * thread. Hence, nothing to be deleted in the scheduler class. The thread
 * itself is not deleted here in the class. Rather, it is deleted in the class
 * Thread itself.
 */

void Scheduler::terminate(Thread *_thread) {
        Console::puts("This is terminating\n");
        yield();
        return;
}

void thread_shutdown() {
        /* This function should be called when the thread returns from the thread function.
           It terminates the thread by releasing memory and any other resources held by the thread. 
           This is a bit complicated because the thread termination interacts with the scheduler.
        */
        
        SYSTEM_SCHEDULER->terminate(current_thread->CurrentThread());
        //assert(FALSE);
        /* Let's not worry about it for now. 
           This means that we should have non-terminating thread functions. 
        */
}
