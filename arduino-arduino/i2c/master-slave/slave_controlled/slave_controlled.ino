/*
 * I2C slave controled.
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
#include<Wire.h>

#define NODE_ADDRESS 1
/**
 * A4 to bus
 * A5 to bus
 * GND to GND Bus
 */
#define NODE_READ_DELAY 100
#define PHOTOREZISTOR_PIN A0
/**
 * Photoresistor conectivity
 * VCC to photresistor first pin
 * photoresistor second pin to A0
 * photoresistor second pin to R=10k first pin
 * R=10K second pin to GND
 */
#define BUFFER_SIZE 3
byte messageFromMaster[BUFFER_SIZE];
byte messageToMaster[255];
String str;
volatile bool hasData = false;
volatile bool sizeRequest = false;

void setup() {
  pinMode(PHOTOREZISTOR_PIN, INPUT);
  Serial.begin(9600);  
  Serial.print("SLAVE #");
  Serial.println(NODE_ADDRESS);
  Wire.onRequest(requestHandler);
  Wire.onReceive(receiveHandler);
  Wire.begin(NODE_ADDRESS);  // Activate I2C network
}

void loop() {
  
}


void receiveHandler(int nrOfBytes) {
  Serial.print("ReceiveHandler rec=");Serial.println(nrOfBytes);Serial.flush();
  for(int i = 0 ; i < BUFFER_SIZE; i++) {
    messageFromMaster[i]='\0';
  }
  
  for(int i = 0; i < nrOfBytes; i++) {
    messageFromMaster[i] = Wire.read();
  }
  Serial.print("<<");Serial.print((char *)messageFromMaster);Serial.println(">>");Serial.flush();
  if (messageFromMaster[0]=='p' && messageFromMaster[1]=='#') {
    str = String(analogRead(PHOTOREZISTOR_PIN));
    Serial.print("Photoresistor value=");Serial.println(str);Serial.flush();
    hasData = true;
  }
}

void requestHandler() {
  if (hasData && !sizeRequest) {
    for(int i = 0 ; i<255;i++) {
      messageToMaster[i]='\0';
    }
    str.getBytes(messageToMaster, 255);
    Wire.write(str.length() + 1);
    sizeRequest = true;
  } else if (hasData && sizeRequest) {
    Serial.println("Start to send to Master");Serial.flush();
    for(int i = 0; i< str.length() + 1;i++) {
      Wire.write(messageToMaster[i]);
    }
    Serial.println("Data sent to Master");Serial.flush();
    hasData = false;
    sizeRequest = false;
  } else {
    Wire.write('s');
  }
}
