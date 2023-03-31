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
// libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// define CS pin for the SD card module
#define SD_CS 4


void setup() {
  // put your setup code here, to run once:
  // start serial for debugging
  Serial.begin(115200);
  Serial.println("serial started");

   // initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {Serial.println("No SD card attached");return;}
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {Serial.println("ERROR - SD card initialization failed!");return;    // init failed}
  // if the data.csv file doesn't exist
  // create a file on the SD card and write the data labels
  File file = SD.open("/data.csv");
  if(!file) {Serial.println("File doens't exist");Serial.println("Creating file...");
  writeFile(SD, "/data.csv", "Reading ID, Date, Time, RTC Temp (°C), Sensor 1 Temp, Sensor 2 Temp, Sensor 3 Temp, FDC 0-0, FDC 0-1, FDC 1-0, FDC 1-1, FDC 2-0, FDC 2-1, FDC 3-0, FDC 3-1, FDC 4-0, FDC 4-1, FDC 5-0, FDC 5-1 \r\n");}else {Serial.println("File already exists");}
  file.close();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void logSDCard() { // write the sensor readings on the SD card
  // each "dataMessage" will be one line in the CSV output
  dataMessage = String(readingID) + "," + fullDate + "," + fullTime + "," + String(rtc.getTemperature()) + "," + String(sensors.getTempC(tSensor1)) + "," + 
  String(sensors.getTempC(tSensor2)) + "," + String(sensors.getTempC(tSensor3)) + "," + capReadsStr + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.csv", dataMessage.c_str());
}

// write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
