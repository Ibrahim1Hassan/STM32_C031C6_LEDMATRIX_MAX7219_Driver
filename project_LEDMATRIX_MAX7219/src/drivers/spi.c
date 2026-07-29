#include "spi.h"
#include "stm32c0xx.h"


void SpiInit(void){
	// TODO: Enable SPI and GPIO ports using RCC
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	RCC->APBENR2 |= RCC_APBENR2_SPI1EN;
	// TODO: Configure GPIOs for MOSI PA7, MISO(not needed), SCK PA1, CS PA4
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
	
	// TODO: Configure CR1 register
	
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
	// TODO: Configure CR2 register
	
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

	SpiSendFrame((0x1U << 8) | 0x0U); // Set digit 0 (Writes 0x0F to the segment register)
	SpiSendFrame((0x2U << 8) | 0x0U); // Set digit 1
	SpiSendFrame((0x3U << 8) | 0x0U); // Set digit 2
	SpiSendFrame((0x4U << 8) | 0x0U); // Set digit 3
	SpiSendFrame((0x5U << 8) | 0x0U); // Set digit 4
	SpiSendFrame((0x6U << 8) | 0x0U); // Set digit 5
	SpiSendFrame((0x7U << 8) | 0x0U); // Set digit 6
	SpiSendFrame((0x8U << 8) | 0x0U); // Set digit 7
}

/**
 * @brief Advances a single lit LED across an 8x8 matrix.
 *        Only clears the previous digit when wrapping to a new column.
 */
void SpiMax7219DrawPattern(void){
    static uint8_t current_digit = 0; // Digits 0 to 7
    static uint8_t current_bit   = 0; // Bit positions 0 to 7

    // 1. If we are starting a new digit, clear the PREVIOUS digit row.
    // (On the very first run, this safely clears Digit 7 which is already blank)
    if (current_bit == 0) {
        // Calculate previous digit, wrapping 0 back to 7
        uint8_t prev_digit = (current_digit == 0) ? 7 : (current_digit - 1);
        uint8_t prev_reg_address = prev_digit + 1;
        
        SpiSendFrame(((uint16_t)prev_reg_address << 8) | 0x00U);
    }

    // 2. Turn ON the active LED (automatically clears previous bits on the SAME digit)
    uint8_t reg_address = current_digit + 1; 
    uint8_t bitmask     = (1U << current_bit);

    SpiSendFrame(((uint16_t)reg_address << 8) | bitmask);

    // 3. Advance to the next position for the NEXT press
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