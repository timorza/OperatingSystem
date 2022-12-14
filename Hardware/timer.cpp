#include "timer.h"
#include "../Interrupts/isr.h"
#include "IO.h"
#include "../Drivers/screenDriver.h"
uint32_t tick = 0;

void restart_timer()
{
	tick = 0;
}

uint32_t get_tick()
{
	return tick;
}

static void timer_callback(registers_t regs)
{
	tick++;
}

void init_timer(uint32_t frequency)
{
   // Firstly, register our timer callback.
   register_interrupt_handler(IRQ0, &timer_callback);



   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   uint32_t divisor = 1193180 / frequency;

   // Send the command byte.
   toutb(0x43, 0x36);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

   // Send the frequency divisor.
   toutb(0x40, l);
   toutb(0x40, h);
   printk(STR"[OK] ", get_char_attributes(0,BLACK,GREEN));
   printk(STR"PIT succesfully set\n");
} 

void wait(uint32_t time)
{
   restart_timer();
   while(true)
   {
      if(get_tick() >= time)
         break;
   }
}