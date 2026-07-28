#include "uart.h"

#include <stdio.h>

#include "main.h"
#include "stm32c0xx.h"

// buffer for uart_dma printf
static volatile uint8_t UartDmaTransmitBuffer[APP_INPUT_STRING_LEN];

void UART2_Init(void) {
		/* enable GPIOA clock port for the UART2  --> already enabled */
		/* set PA2 as Tx and PA3 as Rx for UART2, using GPIOA MODER and AFRL(contain the first 8 ports PB0 till PB7) */
		RCC->APBENR1 |= RCC_APBENR1_USART2EN;
		GPIOA->MODER &= ~GPIO_MODER_MODE2;
		GPIOA->MODER |= GPIO_MODER_MODE2_1;
		GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
		GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0;
		GPIOA->MODER &= ~GPIO_MODER_MODE3;
		GPIOA->MODER |= GPIO_MODER_MODE3_1;
		GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
		GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0;

		// set baud rate for UART1 using UART2 BRR register
		// upon checking RCC register in the watch window (RCC_TypeDef*)0x40021000
		// the UART2 is clocked using HSISYS (12 MHz) -> APB (no division) 
		// from the data sheet with 16 bit oversamlping BRR = ClkSrc / baudrate 
		USART2->BRR = 0x4E2UL;

		/* configure CR1 for oversampling rate, character size (8 bit), and enabling transmit, no parity		8-N-1 */
		USART2->CR1 &= ~USART_CR1_M0;
		USART2->CR1 &= ~USART_CR1_M1;
		USART2->CR1 &= ~USART_CR1_OVER8;
		/* enable USART interrupt generation whenever ORE= 1 or RXNE =1 in the USART_ISR register */
		USART2->CR1 |= USART_CR1_RXNEIE_RXFNEIE;
		/* enable USART2 IRQ */
		NVIC_EnableIRQ(USART2_IRQn);
		/* configure CR2 for number of stop bits and ..etc */
		USART2->CR2 &= ~USART_CR2_STOP_0;
		USART2->CR2 &= ~USART_CR2_STOP_1;

		/* enable UART2 */
		USART2->CR1 |= USART_CR1_UE;

		/* select DMA enable DMAT in CR3 */
		USART2->CR3 |= USART_CR3_DMAT;

		/* set RE to enable the receiver which begins searching for a start bit */
		USART2->CR1 |= USART_CR1_RE;

		/********** DMA Tx CONFIGURATION START	**********/

		/* enable DMA1 clock */
		RCC->AHBENR |= RCC_AHBENR_DMA1EN;

		/* Map USART2_TX (Request 53) to DMA1 Channel1 using DMAMUX1 Channel0 */
		DMAMUX1_Channel0->CCR = (53U & DMAMUX_CxCR_DMAREQ_ID);

		/* set the peripheral address in the DMA_CPARx register */
		DMA1_Channel1->CPAR = (uint32_t)&USART2->TDR;

		/* set the memory address in the DMA_CMARx register */
		DMA1_Channel1->CMAR = (uint32_t)&UartDmaTransmitBuffer;

		/* configure total number of bytes to transfare --> NOT HERE */

		/* configure channel priority, data transfare direction
		circular mode disabled, memory increment mode enabled, peripheral increment mode disabled
		peripheral and memory data size, interrupts disabled */
		DMA1_Channel1->CCR = 0;
		DMA1_Channel1->CCR |= DMA_CCR_MINC;
		DMA1_Channel1->CCR |= DMA_CCR_DIR;

		/* activate the channel by setting EN bit in CCRx register --> NOT HERE  DMA1_Channel1->CCR |= DMA_CCR_EN; */

		/********** DMA Tx CONFIGURATION END	**********/

		/* enable TE transmission enable register, sets an idle frame as first transmission */
		USART2->CR1 |= USART_CR1_TE;
}

void UART2_PrintDma(char const *string) {
		/*    Wait until the UART TC flag is set 
		AND Wait if CNDTR > 0 DMA still has bytes to send. */
		while (!(USART2->ISR & USART_ISR_TC) || (DMA1_Channel1->CNDTR > 0)) {
				static uint32_t wait_counter;
				wait_counter++;
		}

		/* clear TC flag for DMA channel 1 */
		DMA1->IFCR = DMA_IFCR_CGIF1;

		/* deactivate the channel by resetting EN bit in CCRx register */
		DMA1_Channel1->CCR &= ~DMA_CCR_EN;

		/* Calculate string size and copy data simultaneously */
		uint8_t bufferSize = 0;
		while (string[bufferSize] != '\0' && bufferSize < sizeof(UartDmaTransmitBuffer)) {
				UartDmaTransmitBuffer[bufferSize] = (uint8_t)string[bufferSize];
				bufferSize++;
		}

		/* configure buffer size */
		DMA1_Channel1->CNDTR = bufferSize;

		/* clear TC */
		USART2->ICR |= USART_ICR_TCCF;

		/* activate the channel by setting EN bit in CCRx register */
		DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void UART2_SendChar(uint8_t c) {
		while (!(USART2->ISR & USART_ISR_TXE_TXFNF)) {
		}
		USART2->TDR = (c & USART_TDR_TDR);
}

int fputc(int c, FILE *stream) {
		(void)stream;
		if (c == '\n') {
				UART2_SendChar('\r');
		}
		UART2_SendChar((uint8_t)c);
		return c;
}

void USART2_IRQHandler(void) {
		while ((USART2->ISR & USART_ISR_RXNE_RXFNE)) {
				static uint8_t string_index = APP_INPUT_PROMPT_LEN;
				input_string[string_index] = (char)USART2->RDR;
				if (input_string[string_index] == '\r') {
						input_string[string_index] = '\n';
						input_string[string_index + 1U] = '\r';
						input_string[string_index + 2U] = '\0';
						string_index = APP_INPUT_PROMPT_LEN;
						event_signal = UART;
				}
				else {
						string_index++;
						if (string_index == APP_INPUT_STRING_LEN - 4U) {
								event_signal = UART_BUFFER_OVERFLOW;
								string_index = APP_INPUT_PROMPT_LEN;
						}
				}
		}
		/* clear Overrun Event */
		USART2->ICR |= USART_ICR_ORECF;
}