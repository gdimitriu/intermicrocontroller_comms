/*
 * SPI slave controled.
 * Copyright 2018 Gabriel Dimitriu
 *
 * This file is part of intermicrocontroller_comms project.
  
 * intermicrocontroller_comms is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * intermicrocontroller_comms is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with intermicrocontroller_comms; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 */
#include<SPI.h>

#define SS 10
#define MOSI 11
#define MISO 12
#define SCK 13

/**
 * SS( 10) to SS(10) 
 * MOSI (11) to MOSI(11)
 * MISO (12) to MISO(12)
 * SCK (13) to SCK(13)
 * GND to GND Bus
 */

#define BUFFER_LEN 255
char messageTransfer[BUFFER_LEN];
char messageBack[BUFFER_LEN];

#define NODE_READ_DELAY 100
#define PHOTOREZISTOR_PIN A0
/**
 * Photoresistor conectivity
 * VCC to photresistor first pin
 * photoresistor second pin to A0
 * photoresistor second pin to R=10k first pin
 * R=10K second pin to GND
 */
String str;
volatile int pos = 0;
volatile bool hasData = false;
void setup() {
  pinMode(PHOTOREZISTOR_PIN, INPUT);
  Serial.begin(9600);  
  Serial.println("SLAVE #");
  pinMode(MISO,OUTPUT);
  SPCR |= _BV(SPE);
  SPI.attachInterrupt();
  
}

ISR (SPI_STC_vect) {

  byte c = SPDR;

  if (pos < BUFFER_LEN) {
    messageTransfer[pos++] = c;
  }
  if (c == '\0') {
    messageTransfer[pos] = c;
    hasData = true;
  }
}

void loop() {
  if (hasData) {
     processData();
  }
}

void processData() {
    Serial.print("ReceiveHandler rec=");Serial.println(pos);Serial.flush();
    Serial.print("<<");Serial.print((char *)messageTransfer);Serial.println(">>");Serial.flush();
    if (messageTransfer[0]=='p' && messageTransfer[1]=='#') {
      str = String(analogRead(PHOTOREZISTOR_PIN));
      Serial.print("Photoresistor value=");Serial.println(str);Serial.flush();
      hasData = false;
      str.getBytes(messageBack, BUFFER_LEN);
//      SPI.transfer(&messageTransfer,(str.length()+1)*sizeof(char));
      for (int i = 0; i < BUFFER_LEN; i++) {
        if (messageBack[i] != '\0') {
          SPDR = messageBack[i];
          Serial.print(SPDR);
        } else {
          SPDR = messageBack[i];
//          break;
        }
      }
    }
}
