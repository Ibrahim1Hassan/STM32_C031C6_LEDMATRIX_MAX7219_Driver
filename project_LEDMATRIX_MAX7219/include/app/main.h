#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

enum {
    APP_INPUT_PROMPT_LEN = sizeof("Received Input --> ") - 1U,
    APP_INPUT_STRING_LEN = 53U
};

typedef enum {
				INITIAL,
				TIMER_EVENT,
				UART_EVENT,
				BUTTON_EVENT,
				UART_ERROR_EVENT,
				IDLE
     } state_t;

typedef enum {
				TIMER,
				UART,
				UART_BUFFER_OVERFLOW,
				BUTTON,
				NONE
     } event_t;



extern volatile event_t event_signal;
extern char input_string[APP_INPUT_STRING_LEN];
#endif // MAIN_H