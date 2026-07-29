#ifndef DRIVERS_SPI_H
#define DRIVERS_SPI_H

#include <stdint.h>

void SpiInit(void);
void SpiSendFrame(uint16_t DataFrame);
void SpiMax7219Init(void);
void SpiMax7219DrawPattern(void);


#endif // DRIVERS_SPI_H