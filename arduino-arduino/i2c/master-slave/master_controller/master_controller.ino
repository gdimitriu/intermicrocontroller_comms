/*
 * I2C Mastercontroller
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

char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;
bool stopFlag = false;

#define SLAVE_ADDRESS 1
/**
 * A4 to bus
 * A5 to bus
 * GND to GND Bus
 */

#define BUFFER_SIZE 2
byte messageToSlave[BUFFER_SIZE];
byte messageFromSlave[255];

void setup() {
  Serial.begin(9600);  
  Serial.println("MASTER");
  
  Wire.begin();  // Activate I2C link
}

void loop() {

  Serial.println("----------------------------");
  Serial.println("Receive-Request to slave");
  Serial.println("----------------------------");
  Serial.println("MENU:" );
  Serial.println("p# Photoresistor read");
  Serial.println("s# Request Status");
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
    } else if (strcmp(inData,"s") == 0) {
      Wire.requestFrom(SLAVE_ADDRESS,1);
      while(!Wire.available());
      char c = Wire.read();
      Serial.print("Status of the slave:");
      Serial.println(c);
      isValidInput = true;
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}

void getPhotoresitorValue() {
  messageToSlave[0]='p'; //request value
  messageToSlave[1]='#';
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(messageToSlave,2);
  Wire.endTransmission();
  Wire.requestFrom(SLAVE_ADDRESS,1);
  while(!Wire.available());
  int nr = Wire.read();
  Wire.requestFrom(SLAVE_ADDRESS,nr);
  while(!Wire.available());
  for (int i = 0;i < nr; i++) {
    messageFromSlave[i] = Wire.read();
  }
  String strData;
  for (char c : messageFromSlave) strData += c;
  Serial.print("Value received from slave:");
  Serial.print(strData);
  Serial.print(" has lenght received=");Serial.println(nr);
}
