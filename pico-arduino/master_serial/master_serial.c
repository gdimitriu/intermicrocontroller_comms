/*
 * master_serial.c
 *
 *  Created on: July 25, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <string.h>

int main() {
	char buffer[256];
	int index = 0;

	stdio_init_all();
	//initialize UART 1
	uart_init(uart1, 38400);

	// Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
	gpio_set_function(4, GPIO_FUNC_UART);
	gpio_set_function(5, GPIO_FUNC_UART);
	uart_set_translate_crlf(uart1, 1);
	
	volatile bool isWaitingForData = false;
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
			printf("received: %s\n",buffer);
			fflush(stdout);
			if (uart_is_writable(uart1)) {
				uart_puts(uart1,buffer);
				isWaitingForData = true;
			}
		}
		if (isWaitingForData && uart_is_readable(uart1)) {
			char readed = uart_getc(uart1);
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
