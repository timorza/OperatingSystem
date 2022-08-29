#pragma once


#include "../Kernel/types.h"
unsigned char inb (unsigned short port);
void outb (unsigned short port, unsigned char data);
unsigned short inw (unsigned short port);
void outw (unsigned short port, unsigned short data);
void toutb(uint16_t port, uint8_t value);