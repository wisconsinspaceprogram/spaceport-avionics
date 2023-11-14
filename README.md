# AIAA UW-Madison's code for Spaceport
Live telemetry system utilizing LoRa and APRS to report GPS and flight data to ground. Developed by AIAA UW-Madison students for use within a rocket in the Spaceport America Competition.

## Summary
### Purpose
The purpose of this electronic telemetry system is primarily to log flight data (GPS, Altitude, Velocity, Pressure, Temperature, Accelerometer) to an onboard SD card as well as transmit over 433MHz to ground for live data retrieval. Additionally for 2024, this system will control the airbrake system used to achieve target altitude of 10,000' AGL.

### Method
The Flight system is comprised of an MCU, sensor, and RF components stacked together, and a similarly built Ground system will receive the radio signals for display via OLED.

## Dependencies
- Uses Arduino IDE (https://www.arduino.cc/en/software)
- Drivers for ESP32 Feather (or follow [this guide from Adafruit](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/overview))
   - https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html
   - https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

## Hardware
This project makes use of Adafruit's Feather/Wing system for ease of assembly and modularity.

### Main Components
MCU - Adafruit Huzzah32 Feather: https://www.adafruit.com/product/3405  
GPS - Adafruit Ultimate GPS: https://www.adafruit.com/product/3133  
RF - Adafruit RFM95W 433MHz: https://www.adafruit.com/product/3232  
RTC/SD - Adafruit Adalogger: https://www.adafruit.com/product/2922  

### Extra Parts
OLED Display: https://www.adafruit.com/product/4650  
Featherwing Tripler: https://www.adafruit.com/product/3417  
Stacking Headers: https://www.adafruit.com/product/2830
LiPo: https://www.adafruit.com/product/3898 *Spaceport no longer allows LiPo batteries in SRAD systems so this will be replaced by a Li-Ion or other type of battery*  

## Learning Resources
### Electronics
I2C Basics: https://www.circuitbasics.com/basics-of-the-i2c-communication-protocol/  
Serial Basics: https://www.codrey.com/embedded-systems/serial-communication-basics/  
Analog vs Digital Signals: https://www.monolithicpower.com/en/analog-vs-digital-signal  
Microcontrollers: https://www.circuitbasics.com/introduction-to-microcontrolleres/  
Intro to Analog and Digitial Signals: https://www.circuitbasics.com/introduction-to-analog-and-digital-communication-systems/  
Basics of SPI Communication Protocol: https://www.circuitbasics.com/basics-of-the-spi-communication-protocol  
Basics of UART Communication Protocol: https://www.circuitbasics.com/basics-uart-communication/  
Soldering: https://www.circuitbasics.com/how-to-solder/  
How to Build Prototype Electronics:  https://www.circuitbasics.com/how-to-use-prototyping-boards/  
How Analog to Digital Converters Work: https://www.circuitbasics.com/analog-to-digital-converters/  
Voltage Dividers: https://www.circuitbasics.com/what-is-a-voltage-divider/  
PCB Design: https://www.circuitbasics.com/what-is-pcb-design/  
Capacitors: https://www.circuitbasics.com/what-is-a-capacitor/  
Multiplexers: https://www.circuitbasics.com/what-is-a-multiplexer/  
How Crystal Oscillators work: https://www.circuitbasics.com/crystal-oscillators/  
How to Design a PCB Layout: https://www.circuitbasics.com/make-custom-pcb/  

### Programming Hardware
ESP32 in Arduino IDE: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/  
Introduction to Arduino IDE: https://www.theengineeringprojects.com/2018/10/introduction-to-arduino-ide.html  
Getting Started with the ESP32 Development Board: https://randomnerdtutorials.com/getting-started-with-esp32/  
C Programming for microcontrollers: https://www.circuitbasics.com/how-to-write-and-run-a-c-program-on-the-raspberry-pi/  
ESP32 I2C Communication: Set Pins, Multiple Bus Interfaces and Peripherals (Arduino IDE): https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/  

### Arduino
How to Use Variables in Arduino: https://www.circuitbasics.com/using-variables-on-the-arduino/  
Math in Arduino: https://www.circuitbasics.com/how-to-do-math-in-arduino-programs/  
Data types: https://www.circuitbasics.com/using-data-types-in-arduino-programming/  
Conditional Statements: https://www.circuitbasics.com/how-to-use-conditional-statements-in-arduino-programming/  
Using Switch Case Statements in Arduino Programming: https://www.circuitbasics.com/using-switch-case-statements-in-arduino-programming/  
Loops in Arduino Programming: https://www.circuitbasics.com/using-loops-in-arduino-programming/  
User Input in Arduino Programming: https://www.circuitbasics.com/how-to-read-user-input-from-the-arduino-serial-monitor/  
Compound Operators in Arduino: https://www.circuitbasics.com/using-compound-operators-in-arduino-programming/  
Logical Operators in Arduino: https://www.circuitbasics.com/using-logical-operators-in-arduino-programming/  
Arrays in Arduino: https://www.circuitbasics.com/using-arrays-in-arduino-programming/  
Functions in Arduino: https://www.circuitbasics.com/using-functions-in-arduino-programming/  
Libraries for Arduino: https://www.circuitbasics.com/how-to-use-arduino-libraries/  
Classes and Objects in Arduino: https://www.circuitbasics.com/programming-with-classes-and-objects-on-the-arduino/  
Optimizing Arduino Code: https://www.circuitbasics.com/how-to-optimize-arduino-code/  
