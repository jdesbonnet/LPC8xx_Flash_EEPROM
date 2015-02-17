/*
===============================================================================
 Name        : LPC8xx_Flash_EEPROM.c
 Author      : Joe Desbonnet
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC8xx.h"
#endif

#include <cr_section_macros.h>

//#include <sys/types.h>
#include "uart.h"
#include "printf.h"

// Allocate a 64 byte aligned 64 byte block in flash memory for "EEPROM" storage
const uint32_t eeprom_flashpage[64] __attribute__ ((aligned (64))) = {0};


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

// To facilitate tfp_printf()
void myputc (void *p, char c) {
	uart_send_byte(c);
}

void display_eeprom_page () {
    int i,j;
    uint8_t c;
    for (i = 0; i < 4; i++) {
    	tfp_printf ("%04x   ", (int)(&eeprom_flashpage) + (i * 16));
    	for (j = 0; j<16; j++) {
    		tfp_printf (" %02x ", eeprom_flashpage[i*16+j]);
    	}
    	tfp_printf ("\r\n");
    	tfp_printf ("       ");

    	for (j = 0; j<16; j++) {
    		c = eeprom_flashpage[i*16+j];
    		if (c < 32 || c > 126) {
    			tfp_printf("   ");
    		} else {
    			tfp_printf(" %c ",c);
    		}
    	}
    	tfp_printf ("\r\n");
    }
}

int main(void) {

	SwitchMatrix_Init();


    uart_init(9600);

	init_printf(0,myputc);

    uart_send_string_z ("LPC8xx_Flash_EEPROM \r\n");
    uart_send_string_z ("Documentation at https://github.com/jdesbonnet/LPC8xx_Flash_EEPROM\r\n");

    tfp_printf ("flash_page=%x\r\n", eeprom_flashpage);


    display_eeprom_page();

    uart_send_string_z (" W <addr> <val> : write byte to EEPROM bank\r\n");
    uart_send_string_z (" S <addr> <str> : write string starting at <addr>\r\n");
    uart_send_string_z (" R              : read EEPROM bank\r\n");
    uart_send_string_z (" Z              : reboot device\r\n");
    uart_send_string_z (" <addr>         : index in bank from 0 to 40 (hex)");
    uart_send_string_z (" <val>          : byte value from 0 to FF (hex)");

    return 0 ;
}
