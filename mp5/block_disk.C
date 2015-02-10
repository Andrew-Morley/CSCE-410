#include "block_disk.H"
#include "assert.H"
#include "utils.H"
#include "console.H"
#include "simple_disk.H"
#include "scheduler.H"
#include "thread.H"

extern Scheduler * SYSTEM_SCHEDULER;

BlockDisk::BlockDisk(DISK_ID _disk_id, unsigned int _size):
                SimpleDisk(_disk_id, _size) {
}

/*
 * this is a easy and 'smart' implementation. Instead of modifying the actual
 * 'read' and 'write' functions, we just modify the waiting function. All it does
 * is that it puts the thread in the waiting queue and passes the CPU onto another
 * thread. This way, the CPU is not blocked.
 */
 
void BlockDisk::wait_until_ready(){
        while(!is_ready()){
                SYSTEM_SCHEDULER->add(Thread::CurrentThread());
                SYSTEM_SCHEDULER->yield();
        }
}
