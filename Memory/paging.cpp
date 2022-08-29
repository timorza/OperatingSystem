#include "paging.h"
#include "../StandardLibrary/standardLibrary.h"
#include "../Kernel/Panic.h"
#include "memory.h"
// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

page_directory_t* kernel_dir = 0;
page_directory_t* current_dir=0;

uint32_t *frames;
uint32_t nframes;


extern uint32_t placement_address;


// Static function to set a bit in the frames bitset
static void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static uint32_t test_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static uint32_t first_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if ( !(frames[i]&toTest) )
                    return i*4*8+j;
            }
        }
    }
    return 0;
}

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if(page->frame == 0)
    {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1)
            PANIC(STR"No free frames, Memory ran out.");
        set_frame(idx*0x1000);
        page->present = 1;
        page->rw = (is_writeable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
    }
}

void initialise_paging()
{
   // The size of physical memory. For the moment we
   // assume it is 16MB big.
   uint32_t mem_end_page = 0x1000000;


   nframes = mem_end_page / 0x1000;
   frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
   memset((unsigned char*)frames, 0, INDEX_FROM_BIT(nframes));

   // Let's make a page directory.
   kernel_dir = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
   memset((unsigned char*)kernel_dir, 0, sizeof(page_directory_t));
   current_dir = kernel_dir;


   for (int i =0; i < placement_address; i += 0x1000)       
       // Kernel code is readable but not writeable from userspace.
       alloc_frame( get_page(i, 1, kernel_dir), 1, 1);

   // Before we enable paging, we must register our page fault handler.

   register_interrupt_handler(14, page_fault);

   // Now, enable paging!
   switch_page_directory(kernel_dir);
}


// Function to deallocate a frame.
void free_frame(page_t *page)
{
    uint32_t frame;
    if (!(frame=page->frame))
    {
        return;
    }
    else
    {
        clear_frame(frame);
        page->frame = 0x0;
    }
}


void switch_page_directory(page_directory_t *dir)
{
	asm volatile("mov %%eax, %%cr3": :"a"(&dir->tablesPhysical));	
	asm volatile("mov %cr0, %eax");
	asm volatile("orl $0x80000000, %eax");
	asm volatile("mov %eax, %cr0");

    /*
    current_dir = dir;
    asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));*/
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    uint32_t table_idx = address / 1024;
    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else if(make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
    }
}

void page_fault(registers_t regs)
{
    PANIC(STR"Page Fault - Kernel");
}