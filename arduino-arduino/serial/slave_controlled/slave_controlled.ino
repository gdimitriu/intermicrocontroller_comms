/*
 * Serial slave controled.
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
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>
#define RxD 8
#define TxD 9

/**
 * D8 to D9
 * D9 to D8
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
NeoSWSerial BTSerial(RxD, TxD);
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;
bool cleanupBT = false;

void neoSSerial1ISR() {
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}


void setup() {
  pinMode(PHOTOREZISTOR_PIN, INPUT);
  Serial.begin(9600);  
  Serial.println("SLAVE #1");
  BTSerial.begin(38400);  
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

void loop() {
   while(BTSerial.available() > 0) // Don't read unless there you know there is data
   {
     if(index < 19) // One less than the size of the array
     {
        inChar = BTSerial.read(); // Read a character
        if (inChar=='\r' || inChar=='\n' || inChar =='\0' || inChar < 35 || inChar > 122) {
          continue;
        }
        //commands start with a letter capital or small
        if (index == 0 && !((inChar >64 && inChar <91) || (inChar > 96 && inChar<123))) {
          continue;
        }    
        inData[index++] = inChar; // Store it
        inData[index] = '\0'; // Null terminate the string
        if (inChar == '#') {
          break;
        }
     } else {
        makeCleanup();
        cleanupBT = true;
     }
 }
 if (index >= 1) {
  if (inData[index - 1] == '#') {
    getPhotoData();
    makeCleanup();
  } else if (cleanupBT) {
    makeCleanup();
    cleanupBT = false;
  }
 }
}


void getPhotoData() {
  Serial.print("<<");Serial.print((char *)inData);Serial.println(">>");Serial.flush();
  if (inData[0]=='p' && inData[1]=='#') {
    str = String(analogRead(PHOTOREZISTOR_PIN));
    Serial.print("Photoresistor value=");Serial.print(str);Serial.print('#');Serial.flush();
    BTSerial.print(str);
    BTSerial.print('#');
    BTSerial.flush();
  }
}
