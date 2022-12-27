/*
 * master_serial.c
 *
 *  Created on: July 25, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/pio.h>
#include <string.h>
#include "uart_rx.pio.h"
#include "uart_tx.pio.h"
#define RX_PIN 10
#define TX_PIN 11
const uint SERIAL_BAUD = 38400;

int main() {
	char buffer[256];
	int index = 0;

	stdio_init_all();
	
	PIO pioTX = pio0;
    uint smTX = 0;
    uint offsetTX = pio_add_program(pioTX, &uart_tx_program);
    uart_tx_program_init(pioTX, smTX, offsetTX, TX_PIN, SERIAL_BAUD);
	
	PIO pioRX = pio1;
    uint smRX = 0;
    uint offsetRX = pio_add_program(pioRX, &uart_rx_program);
    uart_rx_program_init(pioRX, smRX, offsetRX, RX_PIN, SERIAL_BAUD);
	
	volatile bool isWaitingForData = false;
	char c[20];
	scanf("%s", &c);
	fflush(stdin);
	printf("Receiving start command=%s\n",c);
	fflush(stdout);
	printf("Starting MASTER\n");
	fflush(stdout);
	printf("Enter p# to get the value from slave\n");
	fflush(stdout);
	while (1) {
		if (!isWaitingForData) {
			printf("Input Command\n");
			fflush(stdout);
			scanf("%s",&buffer);
			fflush(stdin);
			printf("received: <<%s>>\n",buffer);
			fflush(stdout);
			if (buffer[0] != 'p' || buffer[1] != '#') {
				printf("Received wrong command\n");
				fflush(stdout);
				continue;
			}
			uart_tx_program_puts(pioTX, smTX, buffer);
			isWaitingForData = true;
		} else {
			char readed = uart_rx_program_getc(pioRX, smRX);
			buffer[index] = readed;
			if (readed == '#') {
				buffer[index + 1] = '\0';
				printf("%s\n", buffer);
				index = 0;
				isWaitingForData = false;
			} else {
				index++;
			}
		}

	}
}
