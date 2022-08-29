#include "../Drivers/screenDriver.h"
#include "../Interrupts/IDT.h"
#include "../Hardware/timer.h"
#include "../Memory/paging.h"
#include "../Drivers/keyboard.h"
#include "../StandardLibrary/standardLibrary.h"
#include "types.h"

extern "C" void _start()
{
	char* bf;
	printk(STR"---Preparing Kernel---\n");
	
	printk(STR"Initialising IDT\n");
	set_idt();
	
	
    
	printk(STR"Initialising PIT\n");
    init_timer((uint32_t)50);

	printk(STR"Initialising keyboard\n");
	init_keyboard();

	printk(STR"Setting up paging, initialising new kernel page directory...\n");

	printk(STR"Waiting for start...");
	wait(100);
	clear_screen();

		
	printk(STR"\n--------------------------------------\nWelcome to the OS of Michael and Timor\n--------------------------------------\n");
	//initialise_paging();
	printk(STR"Enter '1' for further information\n");

	return;
}