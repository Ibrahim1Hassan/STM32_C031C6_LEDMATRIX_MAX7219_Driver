#ifndef DRIVERS_SPI_H
#define DRIVERS_SPI_H

#include <stdint.h>


#define CHAR_ARRAY_BEGIN 48		// begins with the number 0
#define CHAR_ARRAY_END 90			// end with letter Z

#define ADD_SHIFT 8						// address position in the spi data frame



/* Initializes the STM32 SPI1 peripheral and associated GPIO pins. */
void SpiInit(void);

/* Transmits a 16-bit data frame over SPI, actively managing the Chip Select (CS) pin. */
void SpiSendFrame(uint16_t DataFrame);

/* Configures the MAX7219 and wakes the IC. */
void SpiMax7219Init(void);

/* Executes one iteration of the currently active display pattern. */
void SpiMax7219DrawPattern(void);

/* Toggles between the available display patterns and clears the matrix for the new mode. */
void SpiMax7219SwitchPattern(void);



#endif // DRIVERS_SPI_H