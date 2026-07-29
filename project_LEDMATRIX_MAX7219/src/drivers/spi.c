#include "spi.h"
#include "stm32c0xx.h"
#include <stdbool.h>
#include "font8x8_basic.h"
static bool PatternSwitch = 0;
static void SpiMax7219MatrixClear(void);
static void SpiMax7219PatternMovingDot(void);
static void SpiMax7219PatternShuffle(void);
static uint8_t ReverseByte(uint8_t b);

void SpiInit(void){
	// Enable SPI and GPIO ports using RCC
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	RCC->APBENR2 |= RCC_APBENR2_SPI1EN;
	// Configure GPIOs for MOSI PA7, MISO(not needed), SCK PA1, CS PA4
	// PA1->AF0 0x0 and PA7->AF0 0x0 as AF
	GPIOA->MODER |= GPIO_MODER_MODE1_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE1_0;
	GPIOA->MODER |= GPIO_MODER_MODE7_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE7_0;
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL1_0 | GPIO_AFRL_AFSEL1_1 | GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_3);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_3);
	// PA4 as output, push-pull
	// To set PA4 high later GPIOA->BSRR = GPIO_BSRR_BS4; //(no need for bitwise OR)
	// To set PA4 Low GPIOA->BSRR = GPIO_BSRR_BR4;
	GPIOA->MODER |= GPIO_MODER_MODE4_0;
	GPIOA->MODER &= ~GPIO_MODER_MODE4_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;
	
	// Configure CR1 register
	
	// Data bit ordering MSB
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
	// Clock polarity zero when idle
	SPI1->CR1 &= ~SPI_CR1_CPOL;
	// Clock phase rising edge (first clock transition)
	SPI1->CR1 &= ~SPI_CR1_CPHA;
	// BaudRate divider of 1/16 0b011 of the source APB clock (12 MHz)
	// IC max operating freq is 10MHz
//	SPI1->CR1 &= ~(SPI_CR1_BR_2);
	SPI1->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
	// SPI Master Mode
	SPI1->CR1 |= SPI_CR1_MSTR;
	// Software slave mangement enabled (internal slave select NSS disabled)
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
	// Configure CR2 register
	
	// Data frame size 16-bit
	SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3);
	// Enable SPI Peripheral in CR1
	SPI1->CR1 |= SPI_CR1_SPE;
	
	// Set CS High
	GPIOA->BSRR = GPIO_BSRR_BS4;

}
void SpiSendFrame(uint16_t DataFrame){
	// Wait till the BSY flag is cleared in SR register
	while(SPI1->SR & SPI_SR_BSY){
		/* Wait */
	}
	// Monitor TXE of SR register
	while(!(SPI1->SR & SPI_SR_TXE)){
		/* Wait */
	}
	// Set CS Low
	GPIOA->BSRR = GPIO_BSRR_BR4;
	// Write to DR data register
	SPI1->DR = DataFrame;
	// Monitor TXE of SR register
	while(!(SPI1->SR & SPI_SR_TXE)){
		/* Wait */
	}
	// Wait till the BSY flag is cleared in SR register
	while(SPI1->SR & SPI_SR_BSY){
		/* Wait */
	}
	// Set CS High
	GPIOA->BSRR = GPIO_BSRR_BS4;

}

void SpiMax7219Init(void){
	// --- MAX7219 Initialization Sequence ---

	SpiSendFrame((0xFU << 8) | 0x1U); // Display test ON (forces all LEDs on at max brightness)
	SpiSendFrame((0x9U << 8) | 0x0U); // Decode mode: No-decode for all digits (raw bitmask)
	SpiSendFrame((0xBU << 8) | 0x7U); // Scan limit: Display all 8 digits (0 through 7)
	SpiSendFrame((0xAU << 8) | 0x2U); // Intensity: 17/32 duty cycle (medium brightness)
	SpiSendFrame((0xFU << 8) | 0x0U); // Display test OFF (return to normal operation)
	SpiSendFrame((0xCU << 8) | 0x1U); // Shutdown register: 1 = Normal Operation (Wake up)

	// --- Clear Initial Data ---
	
	SpiMax7219MatrixClear();
	
}

static void SpiMax7219MatrixClear(void){
	
	// Loop through all 8 digits and clear them
	for(uint8_t i = 1; i <= 8; i++){
			SpiSendFrame(((uint16_t)i << 8) | 0x00U);
	}
}

/**
 * @brief Advances a single lit LED across an 8x8 matrix.
 *        Only clears the previous digit when wrapping to a new column.
 */
void SpiMax7219DrawPattern(void){
	if(PatternSwitch == 0){
		SpiMax7219PatternMovingDot();
	}

	else{
		SpiMax7219PatternShuffle();
	}
}

void SpiMax7219SwitchPattern(void){
	
	// Toggle the state
	PatternSwitch = !PatternSwitch;
	
	// Clear the display using the correct logic for the new mode
	SpiMax7219MatrixClear();
}

static void SpiMax7219PatternShuffle(void){
	static uint16_t counter = 48;

	for (uint8_t row = 0; row < 8; row++) {
			uint8_t address = row + 1; 
			
			// Fetch the data from the array
			uint8_t row_data = font8x8_basic[counter][row];
			
			// Reverse the bits to fix the hardware mirror effect
			row_data = ReverseByte(row_data); 
			
			// Send the fixed frame
			SpiSendFrame((address << 8) | row_data);
	}

	if (counter == 90) {
			counter = 48; 
	} else {
			counter++;
	}
}

static void SpiMax7219PatternMovingDot(void){
	static uint8_t current_digit = 0; // Digits 0 to 7
	static uint8_t current_bit   = 0; // Bit positions 0 to 7

	//  If we are starting a new digit, clear the PREVIOUS digit row.
	if (current_bit == 0) {
		// Calculate previous digit, wrapping 0 back to 7
		uint8_t prev_digit = (current_digit == 0) ? 7 : (current_digit - 1);
		uint8_t prev_reg_address = prev_digit + 1;
		
		// Clear the LED row
		SpiSendFrame(((uint16_t)prev_reg_address << 8) | 0x00U);
	}

	// Turn ON the active LED (automatically clears previous bits on the SAME digit)
	uint8_t reg_address = current_digit + 1; 
	uint8_t bitmask     = (1U << current_bit);

	SpiSendFrame(((uint16_t)reg_address << 8) | bitmask);

	// Advance to the next position for the NEXT press
	current_bit++;
	
	// When bit wraps past 7, reset bit and advance digit
	if (current_bit > 7) {
		current_bit = 0;
		current_digit++;
		
		// When digit wraps past 7, restart from (Dig 0, Bit 0)
		if (current_digit > 7) {
				current_digit = 0;
		}
	}
}

// Fast 8-bit reversal algorithm
static uint8_t ReverseByte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; // Swap nibbles
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2; // Swap pairs
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1; // Swap adjacent bits
    return b;
}