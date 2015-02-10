// Copyright [2014] <Husain Alshehhi>
// You can use this code as you wish

#include "page_table.H"
#include "console.H"
#include "paging_low.H"

PageTable     * PageTable::current_page_table;
unsigned int    PageTable::paging_enabled;
FramePool     * PageTable::kernel_mem_pool;
FramePool     * PageTable::process_mem_pool;
unsigned long   PageTable::shared_size;

VMPool * PageTable::virtual_regions[];
unsigned int PageTable::pool_used = 0;

// Initialize the PageTable
// Basically setup the page_directory and the page_table
// Pag
PageTable::PageTable() {
  // setting up the page directory
  page_directory  = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);
  page_directory[1023] = ((unsigned long) page_directory )| 3; // mark it as valid
    
  for (unsigned int i = 1; i < ENTRIES_PER_PAGE - 1; i++)
    page_directory[i] = 0 | 2;

  // setting up the page_table
  unsigned long *page_table = (unsigned long *) (process_mem_pool->get_frame() * PAGE_SIZE);
  unsigned long address = 0;
  for (unsigned int i = 0; i < 1024; i++) {
    page_table[i] = address | 7;
    address = address + PAGE_SIZE;
  }

  // make sure that the page_directory's first entry points to the page table
  page_directory[0] = (unsigned long) page_table;
  page_directory[0] |= 3;

  paging_enabled = false;
}

// this function only sets the parameters
void PageTable::init_paging(FramePool* _kernel_mem_pool,
                            FramePool* _process_mem_pool,
                            const unsigned long _shared_size) {

  // set the parameters
  kernel_mem_pool = _kernel_mem_pool;
  process_mem_pool = _process_mem_pool;
  shared_size = _shared_size;

}

// this function defines the current_page_table
void PageTable::load() {
  current_page_table = this;
}

// this function enables paging by writing to a specific register
void PageTable::enable_paging() {
  write_cr3((unsigned long) current_page_table->page_directory);    
  write_cr0(read_cr0() | 0x80000000);
  paging_enabled = true;
}

// this function handles virtual addresses
void PageTable::handle_fault(REGS* _r) {
  unsigned long address = read_cr2();
    
  // Is this address legitimate?
  unsigned int present      = 0;

  for (unsigned int i = 0; i < pool_used; i++) {
    if (virtual_regions[i]->is_legitimate(address) == TRUE)
      present = 1;
  }
    
  if (present == 0) {
    Console::puts("Error! Invalid Address!\n");
    for (;;);
  }
    
  unsigned long* page_dir       = current_page_table->page_directory;
  unsigned long  page_dir_index = (address & DIR_MASK)   >> 22;
  unsigned long  page_tab_index = (address & TABLE_MASK) >> 12;

  // Is there a page? No?
  if ((page_dir[page_dir_index] & 1) == 0) {
    unsigned long* page_tab  = (unsigned long*) (process_mem_pool->get_frame() * PageTable::PAGE_SIZE);
    unsigned long* dir_entry = (unsigned long*) (0xFFFFF << 12);
    dir_entry[page_dir_index]= (unsigned long)  (page_tab) | 7;
  }

  // Cool. We have a page, now we need an entry
  unsigned long* p_frame    = (unsigned long*) (process_mem_pool-> get_frame() * PageTable::PAGE_SIZE);
  unsigned long* page_entry = (unsigned long*) ((0x3FF << 22) + (page_dir_index << 12));
  page_entry[page_tab_index]= (unsigned long)  (p_frame) | 7;
    
}

// this function removes the page
void PageTable::free_page(unsigned long _page_no) {
  process_mem_pool->release_frame(_page_no);
  page_directory[_page_no] = 0 | 7;
  load();
  PageTable::enable_paging();
}

// this function adds a VMPOOl to the pools we have
void PageTable::register_vmpool(VMPool *_pool) {

  // make sure we have enough space to handle
  if (pool_used >= 10) {
    Console::puts("Error! Too many VMPools\n");
    for(;;);
  }
    
  virtual_regions[pool_used] = _pool;
  pool_used++;
}
