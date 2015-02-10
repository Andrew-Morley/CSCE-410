#ifndef _SIMPLE_DISK_H_
#define _SIMPLE_DISK_H_
#include "utils.H"

typedef enum {MASTER = 0, SLAVE = 1} DISK_ID; 
typedef enum {READ = 0, WRITE = 1} DISK_OPERATION;

class SimpleDisk  {
    public:
        /* -- FUNCTIONALITY OF THE IDE LBA28 CONTROLLER */

        DISK_ID      disk_id;            /* This disk is either MASTER or SLAVE */

        unsigned int disk_size;          /* In Byte */

        void issue_operation(DISK_OPERATION _op, unsigned long _block_no);
        /* Send a sequence of commands to the controller to initialize the READ/WRITE 
           operation. This operation is called by read() and write(). */ 
        
        /* -- HERE WE CAN DEFINE THE BEHAVIOR OF DERIVED DISKS */ 

        virtual BOOLEAN is_ready();
        /* Return TRUE if disk is ready to transfer data from/to disk, FALSE otherwise. */

        virtual void wait_until_ready() {
                while (!is_ready()) { /* wait */; }
        }
        /* Is called after each read/write operation to check whether the disk is
           ready to start transfering the data from/to the disk. */
        /* In SimpleDisk, this function simply loops until is_ready() returns TRUE.
           In more sophisticated disk implementations, the thread may give up the CPU
           and return to check later. */

        SimpleDisk(DISK_ID _disk_id, unsigned int _size); 
        /* Creates a SimpleDisk device with the given size connected to the MASTER or 
           SLAVE slot of the primary ATA controller.
           NOTE: We are passing the _size argument out of laziness. In a real system, we would
           infer this information from the disk controller. */

        /* DISK CONFIGURATION */
   
        virtual unsigned int size();
        /* Returns the size of the disk, in Byte. */   


        /* DISK OPERATIONS */

        virtual void read(unsigned long _block_no, char * _buf);
        /* Reads 512 Bytes from the given block of the disk and copies them 
           to the given buffer. No error check! */

        virtual void write(unsigned long _block_no, char * _buf);
        /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif
