/*
===============================================================================
 Name        : LPC8xx_Flash_EEPROM.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC8xx.h"
#endif

#include <cr_section_macros.h>

#include <sys/types.h>
#include "uart.h"


// Allocate a 64 byte aligned 64 byte block in flash memory
const uint32_t eeprom_flashpage[64] __attribute__ ((aligned (64))) = {0};

const char *ascii[] = {"NUL","BLA"};

int _write (int fd, const void *buf, size_t count) {
	int i;
	uint8_t *c;
	c = (uint8_t *)buf;
	for (i = 0; i < count; i++) {
		uart_send_byte(c[i]);
	}
	return count;
}

int _read (int fd, const void *buf, size_t count) {
	int ccount = 0;
	uint8_t *cbuf;
	cbuf = (uint8_t *)buf;
	uint8_t c;

	while (1) {
		while (! ( LPC_USART0->STAT & UART_STAT_RXRDY) );
		c = LPC_USART0->RXDATA;
		cbuf[ccount++] = c;
		if (c=='\r') {
			//cbuf[ccount]=0;
			return ccount;
		}
	}
	return 0;
}

/**
 * UART RXD on SOIC package pin 19
 * UART TXD on SOIC package pin 5
 */
void SwitchMatrix_Init()
{
    /* Enable SWM clock */
    //LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

    /* Pin Assign 8 bit Configuration */
    /* U0_TXD */
    /* U0_RXD */
    LPC_SWM->PINASSIGN0 = 0xffff0004UL;

    /* Pin Assign 1 bit Configuration */
    /* SWCLK */
    /* SWDIO */
    /* RESET */
    LPC_SWM->PINENABLE0 = 0xffffffb3UL;

}

void display_eeprom_page () {
    int i,j;
    uint8_t c;
    for (i = 0; i < 4; i++) {
    	printf ("%04x   ", (int)(&eeprom_flashpage + (i * 16)));
    	for (j = 0; j<16; j++) {
    		printf (" %02x ", eeprom_flashpage[i*16+j]);
    		if (j%4==3) {
    			//printf ("  ");
    		}
    	}
    	//printf (" <-- HEX\r\n");
    	printf ("\r\n");
    	printf ("       ");

    	for (j = 0; j<16; j++) {
    		c = eeprom_flashpage[i*16+j];
    		//printf ("%c   ", (c<32||c>126) ? ' ' : c );
    		printf ("%s ",ascii[c]);
    		if (j%4==3) {
    			//printf ("  ");
    		}
    	}
    	//printf (" <-- ASC\r\n");
    	printf ("\r\n");
    }
}

int main(void) {

	SwitchMatrix_Init();


    uart_init(9600);

    uart_send_string_z ("LPC8xx_Flash_EEPROM \r\n");
    uart_send_string_z ("A demo of using LPC8xx in-application programming (IAP) as a means to persist\r\n");
    uart_send_string_z ("settings etc when CPU is unpowered.. ie a way to emulate the EEPROM function\r\n");
    uart_send_string_z ("found in outher MCUs.\r\n\r\n");
    uart_send_string_z ("Documentation from https://github.com/jdesbonnet/LPC8xx_Flash_EEPROM\r\n");

    printf ("flash_page=%x\r\n", eeprom_flashpage);


    display_eeprom_page();

    uart_send_string_z ("W <addr> <value>  : write byte to EEPROM bank\r\n");
    uart_send_string_z ("S <addr> <string> : write string starting at <addr>\r\n");
    uart_send_string_z ("R                 : read EEPROM bank\r\n");
    uart_send_string_z ("Z                 : reboot device\r\n");

    return 0 ;
}
