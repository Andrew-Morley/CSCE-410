#include "page_table.H"
#include "console.H"
#include "paging_low.H"

PageTable     * PageTable::current_page_table;
unsigned int    PageTable::paging_enabled;
FramePool     * PageTable::kernel_mem_pool;
FramePool     * PageTable::process_mem_pool;
unsigned long   PageTable::shared_size;
unsigned long * PageTable::page_directory;

PageTable::PageTable(){

    // get a frame
    //Console::puts("\nWant a frame for the page_directory");
    page_directory  = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);
    //Console::puts("\nWant a frame for the page_table");
    unsigned long *page_table = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);
    
    unsigned long address=0;
    unsigned int i;
    
    for(i=0; i<1024; i++)
    {
        page_table[i] = address | 3;
        address = address + PAGE_SIZE;
    }

    page_directory[0] = (unsigned long) page_table;
    page_directory[0] |= 3;

    for(i=1; i<ENTRIES_PER_PAGE; i++)
        page_directory[i] = 0 | 2;
    
    paging_enabled = false;
}

void PageTable::init_paging(FramePool* _kernel_mem_pool,
                            FramePool* _process_mem_pool,
                            const unsigned long _shared_size)
{

    //set the parameters
    kernel_mem_pool = _kernel_mem_pool;
    process_mem_pool = _process_mem_pool;
    shared_size = _shared_size;

}

void PageTable::load()
{
    current_page_table = this;
}

void PageTable::enable_paging()
{
    write_cr3((unsigned long) page_directory);    
    write_cr0(read_cr0() | 0x80000000);
    paging_enabled = true;
}

void PageTable::handle_fault(REGS* _r)
{
    // Read the address
    unsigned long address = read_cr2();
    unsigned long* page_dir = current_page_table->page_directory;
    
    // get the page_dir and the page_tab indeces
    unsigned long page_dir_index = (address & DIR_MASK) >> 22;
    unsigned long page_tab_index = (address & TABLE_MASK) >> 12;
    unsigned long* page_entry;

    // get the page_tab address in the page_dir
    unsigned long* page_tab = (unsigned long *) page_dir[page_dir_index];

    // in case there is no page at all!
    if ((page_dir[page_dir_index] & 1) == 0) {
        // create it
        page_tab = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);

        // fill this page table with empty entries
        for(int i=0; i<ENTRIES_PER_PAGE; i++) {
            unsigned long frame = process_mem_pool->get_frame();
            if (frame == 0)
            {
                Console::puts("NO AVAILABLE FRAME\n");
                for(;;);
            }
            page_entry = (unsigned long *) (frame * PAGE_SIZE);                                                    
            page_tab[i] = (unsigned long) (page_entry) | 3;
        }
                                                        
        page_dir[page_dir_index] = (unsigned long) (page_tab);
        page_dir[page_dir_index] |= 3;
    }

    // Now the page is available
    page_entry = (unsigned long *) page_tab[page_tab_index];

    // maybe the entry is not available
    if ((page_tab[page_tab_index] & 1) == 0) {
        // create the entry
        page_entry = (unsigned long *) (process_mem_pool->get_frame() * PAGE_SIZE);                                                      
        page_tab[page_tab_index] = (unsigned long) (page_entry);
        page_tab[page_tab_index] |= 7;
    }
}

