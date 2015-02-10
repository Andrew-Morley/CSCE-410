#include "vm_pool.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

VMPool::VMPool(unsigned long _base_address,
               unsigned long _size,
               FramePool* _frame_pool,
               PageTable* _page_table):
    base_address (_base_address),
    size (_size),
    frame_pool (_frame_pool),
    page_table (_page_table),
    region_no(0)
{
	page_table->register_vmpool(this);
}

unsigned long VMPool::allocate(unsigned long _size)
{
    virtual_region* region = (virtual_region*) base_address;
    if (region_no == 0){
        region->available = 1;
        region->size = size - REGION_SIZE;
        region_no ++;
    }
    unsigned int i;

    // Do we have an empty region?
    unsigned int is_empty = 0;
    for (i=0; i<region_no; i++){
        if (region->available >=1 && region->size >= _size){
            is_empty = 1;
            break;
        }
        // get the next address
        region = (virtual_region*)((unsigned long) region + REGION_SIZE + region->size);
    }
    if (is_empty == 0){
        return 0;
    }

    // We have an empty region! Let's use it
    unsigned long address = (unsigned long) (region) + REGION_SIZE;
    region->available = 0;
    
    if (region->size - _size > REGION_SIZE){
        
        virtual_region* next_region =  (virtual_region*)((unsigned long)(address + _size));
        next_region->available = 1;
        next_region->size = region->size - _size - REGION_SIZE;
        region->size = _size;
        region_no++;
    }

    region = (virtual_region*) base_address;

    for (unsigned int i = 0; i < region_no; ++i){

        region = (virtual_region*)((unsigned long) region + REGION_SIZE + region->size);
                
    }
    return address;
}

void VMPool::release(unsigned long _start_address)
{

    virtual_region* region = (virtual_region*) base_address;

    for (unsigned int i = 0; i < region_no; i++){
        unsigned long address = (unsigned long) region + REGION_SIZE;
        
        // if I missed it, then the address is fake
        if (address > _start_address){
            break;
        }
        
        // if I didn't hit it, go to the next region
        if (address != _start_address){
            region =  (virtual_region*)(unsigned long)(address + region->size);
            continue;
        }

        // I found the region
        region->available = 1;
        
        // can I combine it with the next region if it is empty?
        virtual_region* next_region = (virtual_region*) (address + region->size);
        if (i < region_no + 1 && next_region->available == 1){
            region->size += next_region->size + REGION_SIZE;
            region_no --;
        }
        return;
    }
}

BOOLEAN VMPool::is_legitimate(unsigned long _address)
{
    if((_address > (base_address + size)) || (_address <  base_address))
        return FALSE;
    return TRUE;
}

