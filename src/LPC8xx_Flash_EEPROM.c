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


int _write (int fd, const void *buf, size_t count) {
	int i;
	uint8_t *c;
	c = (uint8_t *)buf;
	for (i = 0; i < count; i++) {
		MyUARTSendByte(c[i]);
	}
	return count;
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

int main(void) {

	SwitchMatrix_Init();


    MyUARTInit(9600);

    printf ("Hello world! %d\r\n",1234);

    // Enter an infinite loop, just incrementing a counter
    int i=0;
    while(1) {
        i++ ;
    }
    return 0 ;
}
