#pragma once
#include "../Kernel/types.h"

void init_timer(uint32_t frequency);
void restart_timer();
uint32_t get_tick();
void wait(uint32_t time);