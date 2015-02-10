#ifndef _BLOCK_DISK_H_
#define _BLOCK_DISK_H_

#include "simple_disk.H"

class BlockDisk: public SimpleDisk {
    protected:
        void wait_until_ready();
    public:
        BlockDisk(DISK_ID _disk_id, unsigned int _size);
};

#endif /* _BLOCK_DISK_H_ */
