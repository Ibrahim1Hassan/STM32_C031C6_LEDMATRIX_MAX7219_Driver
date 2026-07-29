#include "systick.h"

#include "main.h"
#include "bsp.h"
#include "stm32c0xx.h"

static uint32_t volatile l_tickCtr;

void SysTick_Handler(void) {
    static uint32_t start = 0;
		++l_tickCtr;
		/* timer signal */
		if ((l_tickCtr - start) > BSP_TICKS_PER_SEC / 8U) {
				start = l_tickCtr;
				event_signal = TIMER;
    }
}

void SysTick_Init(void) {
		/* initialize SysTick */
		SystemCoreClockUpdate();
		SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);
}

uint32_t SysTick_TickCtr(void) {
    uint32_t tickCtr;

		__disable_irq();
    tickCtr = l_tickCtr;
		__enable_irq();

    return tickCtr;
}

void SysTick_Delay(uint32_t ticks) {
    uint32_t start = SysTick_TickCtr();
		while ((SysTick_TickCtr() - start) < ticks) {
    }
}