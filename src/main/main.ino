// Written by: Kyle Adler,
//
// 03/30/2023
//
// Purpose: Keep time with RTC, log flight data and GPS to SD card and trasmit over LoRa.
// Board: Adafruit ESP32 Feather, serial at 115200.
//
// Code from https://randomnerdtutorials.com as well as example code from adafruit documentation.
//
//
// !!! When powering from LiPo, ensure the polarity is correct per Adafruit's docs,
// don't fry your charging circuit like I did.
// https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/power-management
//
// libraries:

// testsß
// define CS pin for the SD card module
#define SD_CS 4


void setup() {
  // put your setup code here, to run once:
  // start serial for debugging
  Serial.begin(115200);
  Serial.println("serial started");
}

void loop() {
  // put your main code here, to run repeatedly:
