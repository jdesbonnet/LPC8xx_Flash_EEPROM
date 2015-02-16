/*
 * uart.c
 *
 * LPC8xx lightweight UART library
 *
 * Created on: 30 Jun 2013
 * Author: Joe Desbonnet based on NXP example code.
 */

#include <string.h>

#include "LPC8xx.h"
#include "uart.h"

volatile uint8_t uart_rxbuf[UART_BUF_SIZE];
volatile uint32_t uart_rxbuf_index=0;
volatile uint32_t uart_rxbuf_flags=0;

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
void uart_init(uint32_t baudrate)
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
	//NVIC_EnableIRQ(UART0_IRQn);
	//UARTx->INTENSET = UART_STAT_RXRDY | UART_STAT_TXRDY | UART_STAT_DELTA_RXBRK;	/* Enable UART interrupt */

	UARTx->CFG |= UART_CFG_UART_EN;

	return;
}


/**
 * Transmit one byte on UART. If FIFO is full it will block
 * until byte can be transmitted.
 */
void uart_send_byte (uint8_t v) {
	  // wait until data can be written to TXDATA
	  while ( ! (LPC_USART0->STAT & (1<<2)) );
	  LPC_USART0->TXDATA = v;
}

/**
 * Read a CR terminated line. Cannot exceed UART_BUF_SIZE.
 */
int uart_read_line (char *buf) {

	// Wait until EOL flag set by IRQ handler.
	while ( ! (uart_rxbuf_flags & UART_BUF_FLAG_EOL)) {
		__WFI(); // Can reduce power by sleeping between IRQs
	}

	// Copy from uart.c internal buffer to supplied buffer.
	int i=0;
	do {
		buf[i] = uart_rxbuf[i];
	} while (uart_rxbuf[i++]!=0);

	// Reset buffer and flags
	uart_rxbuf_index=0;
	uart_rxbuf_flags=0;
}

/**
 * Wait until all bytes in FIFO have been transmitted. Typical
 * use to ensure that a "reboot" or "sleep" message has been
 * fully transmitted before rebooting/sleeping etc.
 */
void uart_drain () {
	// Wait for TXIDLE flag to be asserted
	while ( ! (LPC_USART0->STAT & (1<<3)) );
}

/**
 * Send zero-terminated string.
 */
void uart_send_string_z (char *buf) {
	while (*buf != 0) {
		uart_send_byte(*buf);
		buf++;
	}
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
			uart_rxbuf_flags |= UART_BUF_FLAG_EOL;
			uart_rxbuf[uart_rxbuf_index]=0; // zero-terminate buffer
		} else if (c>31){
			// echo
			uart_send_byte(c);

			uart_rxbuf[uart_rxbuf_index] = c;
			uart_rxbuf_index++;
			if (uart_rxbuf_index == UART_BUF_SIZE) {
				//MyUARTBufReset();
			}
		}

	} else if (uart_status & UART_STAT_TXRDY ){

		LPC_USART0->INTENCLR = 0x04;
	}

}
