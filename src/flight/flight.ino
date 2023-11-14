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

// gps
#include <Adafruit_GPS.h>


// libraries for SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"


// define A13 for lipo voltage
//pinMode(A13, INPUT);
float lipoVoltage;


// define CS pin for the SD card module
#define SD_CS 4

// define gps serial
#define GPSSerial Serial1
// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false
uint32_t timer = millis();

// variables for logging/time
String dataMessage;


// --- functions for sd card --- 




void logSDCard(String dataMessage) { // write the sensor readings on the SD card
  // each "dataMessage" will be one line in the CSV output
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.csv", dataMessage.c_str());
//  appendFile(SD, "/data.csv", dataMessage);

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

// --- end functions ---



void setup() {
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready

  // put your setup code here, to run once:
  // start serial for debugging
  Serial.begin(115200);
  Serial.println("serial started");

  // gps
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);

  // initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  // if the data.csv file doesn't exist
  // create a file on the SD card and write the data labels
  File file = SD.open("/data.csv");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.csv", "Reading ID, Date, Time, RTC Temp (°C), Sensor 1 Temp, Sensor 2 Temp, Sensor 3 Temp, FDC 0-0, FDC 0-1, FDC 1-0, FDC 1-1, FDC 2-0, FDC 2-1, FDC 3-0, FDC 3-1, FDC 4-0, FDC 4-1, FDC 5-0, FDC 5-1 \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("serial test");

  // gps
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    }

    logSDCard(String(GPS.milliseconds));
    //appendFile(


  lipoVoltage = 2.0*analogRead(A13)/4098.0*3.30;
//  Serial.println(lipoVoltage);
  delay(100); 
}
}
