#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

RF24 radio(9, 10); // CE, CSN
LiquidCrystal_I2C  lcd (0x3f, 20,4);

const byte address[6] = "00001";

void setup() {
  radio.begin();
  Wire.begin();
  lcd.begin();               // start the library
  lcd.backlight();
  lcd.setCursor(0,0);
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

char text[32];
void loop() {
  if (radio.available()) {    
    radio.read(&text, sizeof(text));
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
    char sendData[64];
    sprintf(sendData,"received=%s\n",text);
    radio.stopListening();
    radio.write(sendData,sizeof(sendData));
    radio.startListening();
  }
}
