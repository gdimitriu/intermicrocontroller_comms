/*
 * master_serial.cpp
 *
 *  Created on: Jan 4, 2023
 *      Author: Gabriel Dimitriu 2023s
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/pio.h>
#include <string.h>
#include "uart_rx.pio.h"
#include "uart_tx.pio.h"
#include "FreeMono12pt7b.h"
#include "ili934x.h"

#define RX_PIN 10
#define TX_PIN 11

#define PIN_SCK  2
#define PIN_MOSI 3
#define PIN_MISO 4
//not used
#define PIN_CS   22

#define PIN_DC   6
#define PIN_RST  5

#define SPI_PORT spi0
//do not use 8 and 9 is for uart1

ILI934X *display = NULL;
const uint SERIAL_BAUD = 38400;

int main() {
	char buffer[256];
	char lcdBuffer[256];
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
	
	//initialize lcd
	spi_init(SPI_PORT, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);

    display = new ILI934X(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, 240, 320, R90DEG);
    display->reset();
    display->init();
	display->clear();
	
	volatile bool isWaitingForData = false;
	GFXfont *font = const_cast<GFXfont *>(&FreeMono12pt7b);
	int16_t x;
	int16_t y;
	int16_t minx;
	int16_t miny;
	int16_t maxx;
	int16_t maxy;
	char ch;
	char *str;
	char c[20];
	scanf("%s", c);
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
			scanf("%s",buffer);
			fflush(stdin);			
			printf("received: <<%s>>\n",buffer);
			fflush(stdout);
			sprintf(lcdBuffer,"received: <<%s>>\n",buffer);
			str = lcdBuffer;
			x = 10;
			y = 100;
			minx = 0;
			miny = 0;
			maxx = 0;
			maxy = 0;
			display->clear();
			while ((ch = *str++)) {
				// charBounds() modifies x/y to advance for each character,
				// and min/max x/y are updated to incrementally build bounding rect.
				display->charBounds(ch, &x, &y, &minx, &miny, &maxx, &maxy, font);
				display->drawChar(x,y,ch,display->colour565(255,0,0), font);
			}
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
				str = buffer;
				x = 10;
				y = 100 + 18;
				minx = 0;
				miny = 0;
				maxx = 0;
				maxy = 0;
				while ((ch = *str++)) {
					// charBounds() modifies x/y to advance for each character,
					// and min/max x/y are updated to incrementally build bounding rect.
					display->charBounds(ch, &x, &y, &minx, &miny, &maxx, &maxy, font);
					display->drawChar(x,y,ch,display->colour565(255,0,0), font);
				}
				index = 0;
				isWaitingForData = false;
			} else {
				index++;
			}
		}

	}
}
