/*
 * print.c
 *
 * Very light weight number formatting library.
 *
 * Author: Joe Desbonnet, jdesbonnet@gmail.com
 */


#include "uart.h"

void print_hex8(uint8_t v) {
	int i, h;
	for (i = 4; i >= 0; i -= 4) {
		h = (v >> i) & 0x0f;
		if (h < 10) {
			uart_send_byte('0' + h);
		} else {
			uart_send_byte('A' + h - 10);
		}
	}
}
void print_hex16(uint16_t v) {
	int i, h;
	for (i = 12; i >= 0; i -= 4) {
		h = (v >> i) & 0x0f;
		if (h < 10) {
			uart_send_byte('0' + h);
		} else {
			uart_send_byte('A' + h - 10);
		}
	}
}
