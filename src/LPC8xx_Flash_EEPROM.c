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

    uart_send_string_z ("\r\nEEPROM page:\r\n");


    for (i = 0; i < 4; i++) {
    	print_hex16((uint16_t)(&eeprom_flashpage) + (i * 16));
    	uart_send_string_z("  ");
    	for (j = 0; j<16; j++) {
    		print_hex8(eeprom_flashpage[i*16+j]);
    		uart_send_string_z(" ");
    	}
    	uart_send_string_z("\r\n");
    }
}

int main(void) {

	SwitchMatrix_Init();

    uart_init(9600);

    uart_send_string_z ("LPC8xx_Flash_EEPROM \r\n");
    //uart_send_string_z ("Documentation at https://github.com/jdesbonnet/LPC8xx_Flash_EEPROM\r\n");

    display_eeprom_page();

    uart_send_string_z ("\r\nCommands:\r\n");

    uart_send_string_z (" W <addr> <val> : write byte to EEPROM bank\r\n");
    uart_send_string_z (" R              : read EEPROM bank\r\n");
    uart_send_string_z (" Z              : reboot device\r\n");
    uart_send_string_z (" <addr>         : index in bank from 0 to 40 (hex)\r\n");
    uart_send_string_z (" <val>          : byte value from 0 to FF (hex)\r\n");

    char buf[20];

    while (1) {
        uart_send_string_z ("> ");
    	uart_read_line(buf);
    	if (buf[0]==0) {
    		continue;
    	}
    	switch (buf[0]) {
    	case 'W' : {

    	}
    	case 'S' : {

    	}
    	case 'R' : {
    		display_eeprom_page();
    		break;
    	}
    	case 'Z' : {
    		uart_send_string_z("rebooting...\r\n");
    		uart_drain();
    		NVIC_SystemReset();
    	}
    	default : {
    		uart_send_string_z("invalid command\r\n");
    	}
    	}

    }
    return 0 ;
}
