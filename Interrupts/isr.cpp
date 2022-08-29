#include "isr.h"
#include "../Drivers/screenDriver.h"
#include "../Hardware/IO.h"

isr_t interrupt_handlers[256];

extern "C" void isr_handler(registers_t r)
{
    
}


void register_interrupt_handler(uint8_t n, isr_t handler)
{
    interrupt_handlers[n] = handler;
} 

extern "C" void irq_handler(registers_t regs)
{

    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
   if (regs.int_no >= 40)
       toutb(0xA0, 0x20); // Send reset signal to slave.
    

   // Send reset signal to master. (As well as slave, if necessary).
   toutb(0x20, 0x20);

   if (interrupt_handlers[regs.int_no] != 0)
   {
       isr_t handler = interrupt_handlers[regs.int_no];
       handler(regs);
   }

}