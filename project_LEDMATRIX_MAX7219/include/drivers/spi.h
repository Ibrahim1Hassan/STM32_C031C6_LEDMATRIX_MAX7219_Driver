#ifndef DRIVERS_SPI_H
#define DRIVERS_SPI_H

#include <stdint.h>

void SpiInit(void);
void SpiSendFrame(uint16_t DataFrame);


#endif // DRIVERS_SPI_H