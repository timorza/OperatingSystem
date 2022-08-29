
#include "../Drivers/screenDriver.h"
#include "../Kernel/types.h"

void PANIC(char* msg);

//Panic function, ment to halt the cpu in a case of serious error caused
//to the kernel, from which it can no longer recover safely
void PANIC(char* msg)
{
    printk(STR"[ERROR] ", get_char_attributes(0,WHITE,RED));
    printk(STR "KERNEL PANIC: ",get_char_attributes(0,BLACK,RED));
    printk(STR msg);
    printk(STR "\nThe CPU has been halted, restart system or check the error according to the debug information");
    asm volatile("hlt");    
}