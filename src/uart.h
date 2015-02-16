/*
 * myuart.h
 *
 *  Created on: 30 Jun 2013
 *      Author: joe
 */

#ifndef MYUART_H_
#define MYUART_H_

#ifdef __USE_CMSIS
#include "LPC8xx.h"
#endif

// Need this for bit constants
//#include "lpc8xx_uart.h"

#define UART_BUF_SIZE (80)

/* UART configuration register bit definitions */
#define UART_CFG_UART_EN       (0x01<<0)
#define UART_CFG_DATA_LENG_8	  (0x01<<2)
#define UART_CFG_PARITY_NONE   (0x00<<4)
#define UART_CFG_STOP_BIT_1    (0x00<<6)


/* UART status register bit definition. */
#define UART_STAT_RXRDY         (0x01<<0)
#define UART_STAT_RXIDLE        (0x01<<1)
#define UART_STAT_TXRDY         (0x01<<2)
#define UART_STAT_TXIDLE        (0x01<<3)
#define UART_STAT_CTS           (0x01<<4)
#define UART_STAT_CTS_DELTA     (0x01<<5)
#define UART_STAT_TXINT_DIS     (0x01<<6)

#define UART_STAT_OVRN_ERR      (0x01<<8)
#define UART_STAT_RXBRK         (0x01<<10)
#define UART_STAT_DELTA_RXBRK   (0x01<<11)
#define UART_STAT_START_DETECT  (0x01<<12)
#define UART_STAT_FRM_ERR       (0x01<<13)
#define UART_STAT_PAR_ERR       (0x01<<14)
#define UART_STAT_RXNOISE       (0x01<<15)

// End of line flag
#define UART_BUF_FLAG_EOL (0x01<<0)

void MyUARTInit(uint32_t baudrate);
void MyUARTSendByte(uint8_t v);

#endif /* MYUART_H_ */
