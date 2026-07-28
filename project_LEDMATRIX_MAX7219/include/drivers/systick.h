#ifndef DRIVERS_SYSTICK_H
#define DRIVERS_SYSTICK_H

#include <stdint.h>

void SysTick_Init(void);
uint32_t SysTick_TickCtr(void);
void SysTick_Delay(uint32_t ticks);

#endif // DRIVERS_SYSTICK_H