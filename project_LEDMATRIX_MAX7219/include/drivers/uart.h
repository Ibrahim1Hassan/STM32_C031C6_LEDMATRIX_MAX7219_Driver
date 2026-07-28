#ifndef DRIVERS_UART_H
#define DRIVERS_UART_H

#include <stdint.h>

void UART2_Init(void);
void UART2_SendChar(uint8_t c);
void UART2_PrintDma(char const *string);

#endif // DRIVERS_UART_H