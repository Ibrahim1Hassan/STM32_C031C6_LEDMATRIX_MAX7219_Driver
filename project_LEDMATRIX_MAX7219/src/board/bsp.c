/* Board Support Package (BSP) for the STM32 NUCLEO-C031C6 board */
#include <stdint.h>

#include "bsp.h"
#include "button.h"
#include "systick.h"
#include "uart.h"
#include "stm32c0xx.h"

#define LD4_PIN  5U
#define LD5_PIN  6U

// Local-scope defines -----------------------------------------------------
// LED pins available on the board (just one user LED LD4--Green on PA.5)
#define LD4_PIN  5U

// external LED to be inserted between GND (short leg) and
// D12 (longer leg) on the CN9 connector
#define LD5_PIN  6U

static void BSP_ledGreenInit(void);

void BSP_init(void) {
        /* BSP initialization sequence */
		BSP_ledGreenInit();
		Button_InitB1();
		UART2_Init();
		SysTick_Init();
		SpiInit();
		__enable_irq();
}

void BSP_ledRedOn(void) {
		/* turn LD5 on */
    GPIOA->BSRR = (1U << LD5_PIN);
}

void BSP_ledRedOff(void) {
		/* turn LD5 off */
    GPIOA->BSRR = (1U << (LD5_PIN + 16U));
}

void BSP_ledBlueOn(void) {
}

void BSP_ledBlueOff(void) {
}

void BSP_ledGreenOn(void) {
        /* turn LD4 on */
    GPIOA->BSRR = (1U << LD4_PIN);
}

void BSP_ledGreenOff(void) {
        /* turn LD4 off */
    GPIOA->BSRR = (1U << (LD4_PIN + 16U));
}

static void BSP_ledGreenInit(void) {
        /* enable GPIOA clock port for the LED LD4 */
		RCC->IOPENR |= (1U << 0U);

        /* NUCLEO-C031C6 board has LED LD4 on GPIOA pin LD4_PIN
        // and external LED LD5 on GPIO LD5_PIN
        // set the LED pins as push-pull output, no pull-up, pull-down */
    GPIOA->MODER   &= ~((3U << 2U * LD4_PIN) | (3U << 2U * LD5_PIN));
    GPIOA->MODER   |=  ((1U << 2U * LD4_PIN) | (1U << 2U * LD5_PIN));
    GPIOA->OTYPER  &= ~((1U << LD4_PIN) | (1U << LD5_PIN));
    GPIOA->OSPEEDR &= ~((3U << 2U * LD4_PIN) | (3U << 2U * LD5_PIN));
    GPIOA->OSPEEDR |=  ((1U << 2U * LD4_PIN) | (1U << 2U * LD5_PIN));
    GPIOA->PUPDR   &= ~((3U << 2U * LD4_PIN) | (3U << 2U * LD5_PIN));
}

_Noreturn void assert_failed(char const * const module, int const id);
_Noreturn void assert_failed(char const * const module, int const id) {
    (void)module;
    (void)id;
#ifndef NDEBUG
		/* for debugging, hang on in an endless loop... */
    GPIOA->BSRR = (1U << LD4_PIN);
    for (;;) {
    }
#endif
    NVIC_SystemReset();
}