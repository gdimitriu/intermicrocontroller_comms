#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";

void setup() {
  radio.begin();
  Serial.begin(9600);
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
}
char text[32];
char inChar;
int index = 0;
bool finish = false;
char recv[64];
void loop() {
  while(Serial.available() > 0) {
    if (index < 32) {
      inChar = Serial.read();
      if (inChar == '#') {
        finish = true;
        break;
      }
      text[index++]=inChar;
      text[index] = '\0';
    }    
  }
  if (finish) {
    radio.stopListening();
    radio.write(&text, sizeof(text));
    finish = false;
    index = 0;
    for(int i =0 ;i < 32; i++)
      text[i] = '\0';
    radio.startListening();
  }
  if (radio.available()) {
    radio.read(&recv,sizeof(recv));
    Serial.print(recv);
  }
}
