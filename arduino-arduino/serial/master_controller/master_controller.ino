/*
 * Serial Master controller
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

char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;

#define RxD 8
#define TxD 9

/**
 * D8 to D9
 * D9 to D8
 * GND to GND Bus
 */

#define BUFFER_SIZE 3
char messageToSlave[BUFFER_SIZE];

NeoSWSerial BTSerial(RxD, TxD);

void neoSSerial1ISR() {
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}


void setup() {
  Serial.begin(9600);  
  Serial.println("MASTER");
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

  Serial.println("----------------------------");
  Serial.println("Receive-Request to slave");
  Serial.println("----------------------------");
  Serial.println("MENU:" );
  Serial.println("p# Photoresistor read");
  Serial.println("----------------------------");
   do {
    for (index = 0; index < 20; index++)
    {
       inData[index] = '\0';
    }
    inChar = '0';
    index = 0;
    while(inChar != '#') {
      while( !Serial.available() )
        ; // LOOP...
      while(Serial.available() > 0) // Don't read unless
                                                 // there you know there is data
      {
          if(index < 19) // One less than the size of the array
          {
              inChar = Serial.read(); // Read a character
              inData[index] = inChar; // Store it
              index++; // Increment where to write next
              inData[index] = '\0'; // Null terminate the string
          }
      }
    }
    if (index > 0) {
      inData[index-1] = '\0';
    }
    Serial.print("ReceiveCommand=");Serial.println(inData);
    if (strcmp(inData,"p") == 0) {
      getPhotoresitorValue();
      isValidInput = true;
      makeCleanup();
    } else {
      makeCleanup();
      isValidInput = false;
    }
  } while( isValidInput == true );
}

void getPhotoresitorValue() {
  messageToSlave[0]='p'; //request value
  messageToSlave[1]='#';
  messageToSlave[2] = '\0';
  BTSerial.print(messageToSlave);
  makeCleanup();
  while(inChar != '#') {
    while( !BTSerial.available() )
      ; // LOOP...
    while(BTSerial.available() > 0) // Don't read unless
                                                 // there you know there is data
    {
        if(index < 19) // One less than the size of the array
        {
            inChar = BTSerial.read(); // Read a character
            inData[index] = inChar; // Store it
            index++; // Increment where to write next
            inData[index] = '\0'; // Null terminate the string
        }
    }
  }
  if (index > 0) {
    inData[index-1] = '\0';
    Serial.print("Value received from slave:");
    Serial.println(inData);
  }
}
