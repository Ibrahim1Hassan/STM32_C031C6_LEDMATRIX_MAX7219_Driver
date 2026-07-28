#include "spi.h"
#include "stm32c0xx.h"


void SpiInit(void){
	// TODO: Enable SPI and GPIO ports using RCC
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	RCC->APBENR2 |= RCC_APBENR2_SPI1EN;
	// TODO: Configure GPIOs for MOSI PA7, MISO(not needed), SCK PA5, CS PA4
	// PA5->AF0 0x0 and PA7->AF0 0x0 as AF
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE5_0;
	GPIOA->MODER |= GPIO_MODER_MODE7_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE7_0;
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_1 | GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_3);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_3);
	// PA4 as output, push-pull
	// To set PA4 high later GPIOA->BSRR = GPIO_BSRR_BS4; //(no need for bitwise OR)
	// To set PA4 Low GPIOA->BSRR = GPIO_BSRR_BR4;
	GPIOA->MODER |= GPIO_MODER_MODE4_0;
	GPIOA->MODER &= ~GPIO_MODER_MODE4_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;
	
	// TODO: Configure CR1 register
	// TODO: Configure CR2 register
	



}
void SpiSendFrame(uint16_t DataFrame){
	



}