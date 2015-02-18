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

#include <string.h>
#include "uart.h"
#include "print.h"
#include "parse.h"
#include "iap_driver.h"

// Allocate a 64 byte aligned 64 byte block in flash memory for "EEPROM" storage
const uint8_t eeprom_flashpage[64] __attribute__ ((aligned (64))) = {0};


/**
 * Configure SwitchMatrix to enable UART on pins used for incircuit serial programming.
 *
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

/**
 * Display contents of flash page used for the "EEPROM" to UART.
 */
void display_eeprom_page () {
    int i,j;

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

/**
 * Write 64 byte page to flash.
 *
 * @param data Pointer to 64 byte block of memory to write to flash.
 * This address *must* be in SRAM (writing from flash memory
 * won't work, eg using const defined in the program as param won't work).
 *
 * @return 0 for success, negative value for error.
 */
int32_t eeprom_write (uint8_t *data) {

	uint32_t iap_status;

	iap_init();

	uint32_t flash_page = (uint32_t)&eeprom_flashpage >> 6;
	uint32_t flash_sector = flash_page >> 4;

	// Example code checks MCU part ID, bootcode revision number and serial number. There are some
	// differences in behavior across silicon revisions (in particular to do with ability
	// to erase multiple sectors at the same time). In this case we require to be able to program
	// just one sector, so this does not concern us.

	/* Prepare the page for erase */
	iap_status = (__e_iap_status) iap_prepare_sector(flash_sector, flash_sector);
	if (iap_status != CMD_SUCCESS) return -4;

	/* Erase the page */
	iap_status = (__e_iap_status) iap_erase_page(flash_page, flash_page);
	if (iap_status != CMD_SUCCESS) return -5;

	/* Prepare the page for writing */
	iap_status = (__e_iap_status) iap_prepare_sector(flash_sector, flash_sector);
	if (iap_status != CMD_SUCCESS) return -6;

	/* Write data to page */
	iap_status = (__e_iap_status) iap_copy_ram_to_flash(data,
			(void *)&eeprom_flashpage, 64);
	if (iap_status != CMD_SUCCESS) return -7;

	return 0;
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

    int argc;
    char *args[4];
    char buf[20];
    char *s;

    // 64 byte block in SRAM: needed as eeprom_write() param must be in SRAM.
	uint8_t rambuf[64];

    while (1) {

    	// Display prompt and wait for CR terminated line of input
        uart_send_string_z ("> ");
    	uart_read_line(buf);

    	// Ignore empty lines
    	if (buf[0]==0) {
    		continue;
    	}

    	// Split command line into space separated args
		args[0] = s = buf;
		argc = 1;
		while (*s != 0) {
			if (*s == ' ') {
				*s = 0;
				args[argc++] = s+1;
			}
			s++;
		}


    	switch (buf[0]) {
    	case 'W' : {
    		// Expecting format
    		// W <addr> <val>
    		// for example:
    		// W 03 5F
    		int addr = parse_hex(args[1]);
    		int val = parse_hex(args[2]);

    		if (addr >= 0x40) {
    			uart_send_string_z("ERR: addr too big\r\n");
    			continue;
    		}
    		if (val > 0xFF) {
    			uart_send_string_z("ERR: val too big\r\n");
    			continue;
    		}

    		// Copy from flash page to SRAM buffer
    		memcpy(rambuf,eeprom_flashpage,64);

    		// Set byte
    		rambuf[addr] = val;

    		// Write back from SRAM to flash
    		eeprom_write(rambuf);

    		break;
    	}

    	case 'R' : {
    		display_eeprom_page();
    		break;
    	}
    	case 'Z' : {
    		uart_send_string_z("rebooting!\r\n");
    		uart_drain();
    		NVIC_SystemReset();
    	}
    	default : {
    		uart_send_string_z("ERR: invalid cmd\r\n");
    	}
    	}

    }
    return 0 ;
}
