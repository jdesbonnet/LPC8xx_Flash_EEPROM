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
#include "myuart.h"


int _write (int fd, const void *buf, size_t count) {
	int i;
	uint8_t *c;
	c = (uint8_t *)buf;
	for (i = 0; i < count; i++) {
		MyUARTSendByte(c[i]);
	}
	return count;
}


int main(void) {

    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;

    printf ("Hello world! %d\r\n",1234);

    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
