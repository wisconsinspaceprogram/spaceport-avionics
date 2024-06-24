#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_ADXL345_U.h>
#include <RH_RF95.h>
#include <math.h>

int servoPin = 13;        // PWM pin connected to the servo
int startPosition = 105;  // Starting position of the servo (in degrees)
int targetPosition = 80;  // Target position of the servo (startPosition - 25 degrees)
int delayTime = 1000;     // Delay time in milliseconds (1 second)

String fileName = "/spaceport2024.txt";

float startHeight_ft;
float latitude;
float longitude;
float altitude_meters;
float pressure;
float temperature;
float targetHeight_ft;  //target height adjusted after current height is found, from ft -> m
float projection;
int servoDeg = 0;
boolean engageFlaps = false;
float flapPosition = 0;  // 1 for open, 0 for closed
//Needed for velocity
unsigned long lastCalc;
float lastHeight;
float velocity;
float acceleration;

sensors_event_t event;

// Servo
Servo servo1;

// GPS
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

// Accelerometer
Adafruit_ADXL345_Unified accelerometer = Adafruit_ADXL345_Unified(0x53);

// Barometric Pressure Sensor
Adafruit_MPL3115A2 altimeter;

// RTC and SD card
RTC_PCF8523 rtc;
const int chipSelect = 33;
File logFile;

// Radio Set Up
#define RFM95_INT 14  // E
#define RFM95_CS 32   // D
#define RFM95_RST 15  // C
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

float lipoVoltage;


void setup() {
#define buzzer 27
  pinMode(27, OUTPUT);  // Buzzer

  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial)
    ;

  // Initialize GPS
  Serial.println("Initializing GPS");
  //GPSSerial.begin(9600);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);

  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(400);


  // Initialize accelerometer
  Serial.println("Initializing Accelerometer");
  if (!accelerometer.begin()) {
    Serial.println("Could not start accelerometer!");
    while (1)
      ;
  }
  accelerometer.setRange(ADXL345_RANGE_2_G);

  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);

  // Initialize barometric pressure sensor
  Serial.println("Initializing Altimeter");
  if (!altimeter.begin()) {
    Serial.println("Could not start barometric pressure sensor!");
    while (1)
      ;
  }
  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  altimeter.setSeaPressure(1010.16);
  altimeter.setMode(MPL3115A2_ALTIMETER);
  //altimeter.startOneShot(); // get first data

  // Initialize RTC
  Serial.println("Initializing RTC");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Initialize SD card
  Serial.println("Initializing SD Card Reader");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial) delay(1);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1)
      ;
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1)
      ;
  }
  rf95.setTxPower(23, false);



  // Serial.println("Opening Log File");
  // logFile = SD.open(fileName, FILE_WRITE);
  // if (!logFile) {
  //   Serial.println("Failed to open file");
  //   while (1);
  // }

  // Write header to the file
  logFile = SD.open(fileName, FILE_WRITE);
  logFile.println("Time,Latitude,Longitude,Acceleration (X),Projection,Flaps,Temperature,Altitude,Pressure");
  logFile.flush();
  logFile.close();

  
  servo1.attach(servoPin);  // Attaches the servo on the specified pin to the servo object
  servo1.write(startPosition);  // Move servo to start position
  delay(3000);  // Wait 3 seconds for servo to reach the starting position


  altimeter.setMode(MPL3115A2_ALTIMETER);
  altimeter.startOneShot();
  while (!altimeter.conversionComplete()) {
  }
  
  startHeight_ft = altimeter.getLastConversionResults(MPL3115A2_ALTITUDE) * 3.28084;
  targetHeight_ft = startHeight_ft + 10000;
}

void flaps() {
  velocity = (altitude_meters - lastHeight) / ((lastCalc - millis()) * 0.001);
  projection = altitude_meters - ((velocity * velocity ) / 2 * (event.acceleration.x + 10.1));


  if (projection > targetHeight_ft) {
    // extend flaps
    servoDeg = max(servoDeg - 5, 80);
    servo1.write(servoDeg);
    flapPosition = 1 - servoDeg - 80 / 25;
  } else if (projection < targetHeight_ft) {
    // retract flaps
    servoDeg = min(servoDeg + 5, 105);
    servo1.write(servoDeg);
    flapPosition = 1 - servoDeg - 105 / 25;
  }

  // Log the data
  logData();

  lastCalc = millis();
  lastHeight = altitude_meters;
}


// Function to convert GPS coordinates from ddmm.mmmmm format to decimal degrees
float convertToDecimalDegrees(float coordinate) {
  int degrees = (int)(coordinate / 100);
  float minutes = coordinate - (degrees * 100);
  return degrees + (minutes / 60);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {
  


  // Read data from the GPS module

  while (GPSSerial.available()) {
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
      if (!GPS.parse(GPS.lastNMEA())) {
        Serial.println("Failed to parse GPS data");
      } else {
        // Convert latitude and longitude to decimal degrees
        //Does not account for N/S and E/W negatives
        latitude = convertToDecimalDegrees(GPS.latitude);
        longitude = convertToDecimalDegrees(GPS.longitude);
        //Serial.println("New GPS data parsed successfully");
      }
    }
  }


  // Read data from accelerometer
  accelerometer.getEvent(&event);


  //altitude = altimeter.getAltitude();
  altimeter.setMode(MPL3115A2_ALTIMETER);
  altimeter.startOneShot();
  while (!altimeter.conversionComplete()) {
    if (engageFlaps) flaps();
  }
  altitude_meters = altimeter.getLastConversionResults(MPL3115A2_ALTITUDE);
  if (!engageFlaps) {
    if ((event.acceleration.x + 10.1) < -5 && altitude_meters > (startHeight_ft/3.28084) + 500) { //Units wrong during test launch. Fixed now
      delay(2000);
      engageFlaps = true;
      lastHeight = altitude_meters;
      lastCalc = millis() - 1;
    }
  }


  pressure = altimeter.getPressure();
  temperature = altimeter.getTemperature() * 9 / 5 + 32;

  // Transmitting Data
  char radiopacket[63] = "                                                              ";
  dtostrf(latitude, 10, 6, radiopacket);
  radiopacket[10] = ',';
  dtostrf(longitude, 10, 6, radiopacket + 11);
  radiopacket[21] = ',';
  dtostrf((event.acceleration.x + 10.1), 5, 2, radiopacket + 22);
  radiopacket[27] = ',';
  dtostrf(temperature, 6, 2, radiopacket + 28);
  radiopacket[34] = ',';
  dtostrf(altitude_meters * 3.28084, 8, 2, radiopacket + 35);
  radiopacket[43] = ',';
  dtostrf(pressure, 7, 2, radiopacket + 44);
  radiopacket[51] = ',';
  dtostrf(2.0 * analogRead(A13) / 4098.0 * 3.30, 4, 2, radiopacket + 52);
  radiopacket[56] = ',';
  itoa(packetnum++, radiopacket + 57, 10);
  radiopacket[62] = '\0';
  Serial.println(radiopacket);

  rf95.send((uint8_t *)radiopacket, 63);
  rf95.waitPacketSent();
  logData();
  delay(10);
}

void logData() {
  logFile = SD.open(fileName, FILE_APPEND);
  if (logFile) {
    Serial.println("logging data...");
    logFile.print(rtc.now().timestamp());
    logFile.print(",");
    logFile.print(latitude, 6);
    logFile.print(",");
    logFile.print(longitude, 6);
    logFile.print(",");
    logFile.print((event.acceleration.x + 10.1));
    logFile.print(",");
    logFile.print(projection);
    logFile.print(",");
    logFile.print(flapPosition);
    logFile.print(",");
    logFile.print(temperature);
    logFile.print(",");
    logFile.print(altitude_meters);
    logFile.print(",");
    logFile.print(pressure);
    logFile.println("");
    logFile.flush();
    logFile.close();
  }
}
