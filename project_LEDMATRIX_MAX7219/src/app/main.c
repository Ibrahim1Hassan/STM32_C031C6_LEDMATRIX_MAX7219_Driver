#include <stdbool.h>

#include "main.h"
#include "bsp.h"
#include "uart.h"
#include "stm32c0xx.h"

static state_t main_state = INITIAL;
volatile event_t event_signal = NONE;
char input_string[APP_INPUT_STRING_LEN] = "Received Input --> ";
static void clear_event_signal(void);
static void clear_event_signal(void)
{
		if (event_signal != NONE) {
			event_signal = NONE;
		}
}

int main(void) {
    BSP_init();

    while (1) {
        __disable_irq();

				static bool led_state = 0u;
        switch (main_state) {
            case INITIAL:
							main_state = IDLE;
							break;

            case TIMER_EVENT:
							if (led_state == 0u) {
									BSP_ledGreenOn();
									SpiMax7219DrawPattern();
									led_state = 1u;
									UART2_PrintDma("LED Green ON\n\r");
							}
							else {
									BSP_ledGreenOff();
									led_state = 0u;
									UART2_PrintDma("LED Green OFF\n\r");
							}
							main_state = IDLE;
							break;

					case UART_EVENT:
						UART2_PrintDma(input_string);
						main_state = IDLE;
						break;

					case UART_ERROR_EVENT:
						UART2_PrintDma("Please Enter a maximum of 30 characters\n\r");
						main_state = IDLE;
						break;

					case BUTTON_EVENT:
						SpiMax7219DrawPattern();
						UART2_PrintDma("Button Pressed\n\r");
						main_state = IDLE;
						break;

					case IDLE:
						if (event_signal == UART) {
								clear_event_signal();
								main_state = UART_EVENT;
						}
						else if (event_signal == UART_BUFFER_OVERFLOW) {
								clear_event_signal();
								main_state = UART_ERROR_EVENT;
						}
						else if (event_signal == TIMER) {
								clear_event_signal();
								main_state = TIMER_EVENT;
						}
						else if (event_signal == BUTTON) {
								clear_event_signal();
								main_state = BUTTON_EVENT;
						}
						else {
								/* DO NOTHING */
						}
						break;

            default:
							while (1) {
									/* ERROR */
							}
							break;
        }

				__enable_irq();
    }
}