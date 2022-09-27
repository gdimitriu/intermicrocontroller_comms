/*
 * SPI Master controller
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

char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;
bool stopFlag = false;

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

void setup() {
  Serial.begin(9600);  
  Serial.println("MASTER");
  
  SPI.begin();  // Activate SPI link
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  digitalWrite(SS,HIGH);
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
    while (inChar != '#') {
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
    Serial.print("ReceiveCommand=");Serial.println(inData);
    if (strcmp(inData,"p#") == 0) {
      getPhotoresitorValue();
      isValidInput = true;
    }  else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}

void getPhotoresitorValue() {
  for (int i = 0; i < BUFFER_LEN; i++)
    messageTransfer[i] = '\0';
  messageTransfer[0]='p'; //request value
  messageTransfer[1]='#';
  messageTransfer[2]= '\0';
  digitalWrite(SS, LOW);
  SPI.transfer(&messageTransfer,3*sizeof(char));
  String strData;
  for (char c : messageTransfer) strData += c;
  Serial.print("Value received from slave:");
  Serial.print(strData);
  Serial.print(" has lenght received=");Serial.println(strlen(messageTransfer));
}
