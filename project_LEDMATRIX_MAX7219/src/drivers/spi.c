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