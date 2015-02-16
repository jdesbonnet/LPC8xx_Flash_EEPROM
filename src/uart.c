/*
 * myuart.c
 *
 *  Created on: 30 Jun 2013
 *      Author: joe
 */

#include <string.h>

#include "LPC8xx.h"			/* LPC8xx Peripheral Registers */

#include "uart.h"


volatile uint8_t uart_rxbuf[UART_BUF_SIZE];
volatile uint32_t uart_rxi=0;
volatile uint32_t uart_buf_flags=0;

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART port, setup pin select,
**						clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate
** Returned value:		None
**
*****************************************************************************/
void MyUARTInit(uint32_t baudrate)
{

	LPC_USART_TypeDef *UARTx = LPC_USART0;

	uint32_t UARTSysClk;

	//UARTClock_Init( UARTx );
	LPC_SYSCON->UARTCLKDIV = 1;     /* divided by 1 */
	NVIC_DisableIRQ(UART0_IRQn);

	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<14);


	/* Peripheral reset control to UART, a "1" bring it out of reset. */
	LPC_SYSCON->PRESETCTRL &= ~(0x1<<3);
	LPC_SYSCON->PRESETCTRL |= (0x1<<3);
	//lpc8xx_peripheral_reset(3);


	UARTSysClk = SystemCoreClock/LPC_SYSCON->UARTCLKDIV;
	UARTx->CFG = UART_CFG_DATA_LENG_8|UART_CFG_PARITY_NONE|UART_CFG_STOP_BIT_1; /* 8 bits, no Parity, 1 Stop bit */
	UARTx->BRG = UARTSysClk/16/baudrate-1;	/* baud rate */
		/*
			Integer divider:
			BRG = UARTSysClk/(Baudrate * 16) - 1
			Frational divider:
			FRG = ((UARTSysClk / (Baudrate * 16 * (BRG + 1))) - 1) where FRG = (LPC_SYSCON->UARTFRDADD + 1) / (LPC_SYSCON->UARTFRDSUB + 1)
		*/
		/*	(1) The easist way is set SUB value to 256, -1 encoded, thus SUB register is 0xFF.
				(2) In ADD register value, depending on the value of UartSysClk, baudrate, BRG register value, and SUB register value, be careful
				about the order of multiplyer and divider and make sure any multiplyer doesn't exceed 32-bit boundary and any divider doesn't get
				down below one(integer 0).
				(3) ADD should be always less than SUB. */
	LPC_SYSCON->UARTFRGDIV = 0xFF;
	LPC_SYSCON->UARTFRGMULT = (((UARTSysClk / 16) * (LPC_SYSCON->UARTFRGDIV + 1)) / (baudrate * (UARTx->BRG + 1))) - (LPC_SYSCON->UARTFRGDIV + 1);

	UARTx->STAT = UART_STAT_CTS_DELTA | UART_STAT_DELTA_RXBRK;		/* Clear all status bits. */

	// Enable UART interrupt
	NVIC_EnableIRQ(UART0_IRQn);

	UARTx->INTENSET = UART_STAT_RXRDY | UART_STAT_TXRDY | UART_STAT_DELTA_RXBRK;	/* Enable UART interrupt */
	UARTx->CFG |= UART_CFG_UART_EN;
	return;
}


void MyUARTSendByte (uint8_t v) {
	  // wait until data can be written to TXDATA
	  while ( ! (LPC_USART0->STAT & (1<<2)) );
	  LPC_USART0->TXDATA = v;
}

void MyUARTSendDrain () {
	// Wait for TXIDLE flag to be asserted
	while ( ! (LPC_USART0->STAT & (1<<3)) );
}

void UART0_IRQHandler(void)
{

	uint32_t uart_status = LPC_USART0->STAT;

	// UM10601 §15.6.3, Table 162, p181. USART Status Register.
	// Bit 0 RXRDY: 1 = data is available to be read from RXDATA
	// Bit 2 TXRDY: 1 = data may be written to TXDATA
	if (uart_status & UART_STAT_RXRDY ) {

		uint8_t c = LPC_USART0->RXDATA;

		// If CR flag EOL
		if (c=='\r') {
			uart_buf_flags |= UART_BUF_FLAG_EOL;
			uart_rxbuf[uart_rxi]=0; // zero-terminate buffer
		} else if (c>31){
			// echo
			MyUARTSendByte(c);

			uart_rxbuf[uart_rxi] = c;
			uart_rxi++;
			if (uart_rxi == UART_BUF_SIZE) {
				//MyUARTBufReset();
			}
		}

	} else if (uart_status & UART_STAT_TXRDY ){

		LPC_USART0->INTENCLR = 0x04;
	}

}
