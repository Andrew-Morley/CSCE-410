#include "frame_pool.H"
#include "console.H"

FramePool::FramePool(unsigned long _base_frame_no,
                     unsigned long _nframes,
                     unsigned long _info_frame_no){

    unsigned int i;
    base_frame_no = _base_frame_no;
    nframes = _nframes;
    if (_info_frame_no == 0)
        _info_frame_no = base_frame_no;
    info_frame_no = _info_frame_no;

    bitmap = (unsigned int *) (info_frame_no * FRAME_SIZE);

    // clear all the entries in that 4kB
    for (i=0; i<512; i++){
        bitmap[i] = 0;
    }
    
    mark_inaccessible(info_frame_no, 1);
    //Console::puts("Done with Frame Constructor");
}

unsigned long FramePool::get_frame()
{
    //Console::puts("\nI am in get_free\n");
    unsigned int i, j, value;
    for (i = 0; i < 512; i++)
    {
        value = bitmap[i];
        if (value != 0xFF)
            break;
    }
    
    //Console::puts("\nI am out of the first loop with value i=");
    //Console::putui(i);
    if (value == 0xFF)
        return 0;

    //Console::puts("\nI am pass the value check!\nValue=");
    //Console::putui(value);
    for (j = 0; j < 8; j++)
    {
        unsigned int temp = value & (0x1 << j);
        //Console::puts("\nThe vale of temp is ");
        //Console::putui(temp);
        if (temp == 0)
        {
            //Console::puts("\nI am done with value j=");
            //Console::putui(j);
            //Console::puts("\nbase_frame_no = ");
            //Console::putui((unsigned int) base_frame_no);
            unsigned int ret = i*8 + j + base_frame_no;
            mark_inaccessible(ret, 1);
            //Console::puts ("\nI am returning as free frame: ");
            //Console::putui (ret);
            return ret;
        }
    }

    Console::puts("\nError: No Frame is returned!"); for (;;);
}

void FramePool::mark_inaccessible(unsigned long _base_frame_no,
                                  unsigned long _nframes){
    //Console::puts("\nMarking frame ");
    //Console::putui(_base_frame_no);
    //Console::puts(" as inaccessible for ");
    //Console::putui(_nframes);
    //Console::puts(" many frames. ");
    //Console::putui( (unsigned int) base_frame_no);
    _base_frame_no -= base_frame_no;
    unsigned long i, s, r, j;
    for (i = 0; i < _nframes; i++){
        // I want to mark j as inaccessible
        j = _base_frame_no + i;
        s = (unsigned long) j / 8;
        r = _base_frame_no%8;
        bitmap[s] |= 1 << r;
    }
}

void FramePool::release_frame(unsigned long _frame_no){
    //unsigned long s = (unsigned long) _frame_no / 32;
    //unsigned long r = _frame_no - s * 32;
    //bitmap[s] &= ~(1 << (32-r-1));
}
