#include "button.h"

#include "main.h"
#include "stm32c0xx.h"

// Button pins available on the board (just one user Button B1 on PC.13)
#define B1_PIN   13U

void Button_InitB1(void) {
		// enable GPIOC clock port for the Button B1
    RCC->IOPENR |= (1U << 2U);

    // configure Button B1 (PC.13) pins as input, no pull-up, pull-down
    GPIOC->MODER   &= ~(3U << 2U * B1_PIN);
    GPIOC->OSPEEDR &= ~(3U << 2U * B1_PIN);
    GPIOC->OSPEEDR |=  (1U << 2U * B1_PIN);
    GPIOC->PUPDR   &= ~(3U << 2U * B1_PIN);

    // configure Button B1 interrupt as falling edge
    EXTI->EMR1 &= ~(1U << B1_PIN);
    EXTI->IMR1 |= (1U << B1_PIN);
    EXTI->RTSR1 &= ~(1U << B1_PIN);
    EXTI->FTSR1 |= (1U << B1_PIN);
		// EXTI port C line 13
    EXTI->EXTICR[3] &= ~(7U << 8);
    EXTI->EXTICR[3] |= (2U << 8);

		NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void) {
	__disable_irq();
	// Clear the pending flag
  EXTI->FPR1 |= EXTI_FPR1_FPIF13;
	/* send BUTTON signal */
	event_signal = BUTTON;
	/* counter for ISR */
	static uint16_t button_int_ctr;
	button_int_ctr++;
	__enable_irq();
}