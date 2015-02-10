#include "frame_pool.H"
#include "console.H"

FramePool* FramePool::kernel_mem_pool = 0;
FramePool* FramePool::process_mem_pool = 0;

FramePool::FramePool(unsigned long _base_frame_no,
                     unsigned long _nframes,
                     unsigned long _info_frame_no)
{

    unsigned int i;
    base_frame_no = _base_frame_no;
    nframes = _nframes;
    if (_info_frame_no == 0)
        _info_frame_no = base_frame_no;
    info_frame_no = _info_frame_no;

    bitmap = (unsigned int *) (info_frame_no * FRAME_SIZE);

    // clear all the entries in that 4kB
    for (i=0; i<512; i++)
    {
        bitmap[i] = 0;
    }
    
    mark_inaccessible(info_frame_no, 1);

    // assigned if this is the kernel or the process framepool
    if (kernel_mem_pool == 0)
        kernel_mem_pool = this;
    else
        process_mem_pool = this;
}

unsigned long FramePool::get_base_frame_no()
{
    return base_frame_no;
}

unsigned long FramePool::get_frame()
{

    // Make sure there there is a empty frame
    unsigned int i, j, value;
    for (i = 0; i < 512; i++)
    {
        value = bitmap[i];
        if (value != 0xFF)
            break;
    }
    
    if (value == 0xFF)
        return 0;

    // Find the location of that frame in the array
    for (j = 0; j < 8; j++)
    {
        unsigned int temp = value & (0x1 << j);
        if (temp == 0)
        {
            unsigned int ret = i*8 + j + base_frame_no;
            mark_inaccessible(ret, 1);
            return ret;
        }
    }

    Console::puts("\nError: No Frame is returned!"); for (;;);
}

void FramePool::mark_inaccessible(unsigned long _base_frame_no,
                                  unsigned long _nframes)
{

    _base_frame_no -= base_frame_no;
    unsigned long i, s, r, j;
    for (i = 0; i < _nframes; i++)
    {
        // I want to mark j as inaccessible
        j = _base_frame_no + i;
        s = (unsigned long) j / 8;
        r = _base_frame_no%8;
        bitmap[s] |= 1 << r;
    }
}

bool FramePool::includes_frame(unsigned long _base_frame_no)
{
    if (base_frame_no <= _base_frame_no && _base_frame_no < base_frame_no + nframes)
        return true;
    return false;
}

void FramePool::release_frame(unsigned long _frame_no)
{
    FramePool* temp_fp;
    if (kernel_mem_pool->includes_frame(_frame_no))
        temp_fp = kernel_mem_pool;
    else
        temp_fp = process_mem_pool;

    _frame_no -= temp_fp->get_base_frame_no();
    unsigned long s = (unsigned long) _frame_no / 8;
    unsigned long r = _frame_no % 8;
    temp_fp->bitmap[s] &= ~(0x1 << r);
}
