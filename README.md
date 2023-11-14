# spaceport-telemetry
Live telemetry system utilizing LoRa and APRS to report GPS and flight data to ground.

Developed by AIAA UW-Madison students for use within a rocket in the Spaceport America Competition.

## Dependencies
- Uses Arduino IDE (https://www.arduino.cc/en/software)
- Drivers for ESP32 Feather (or follow [this guide from Adafruit](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/overview))
   - https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html
   - https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

## Hardware
This project makes use of Adafruit's Feather/Wing system for ease of assembly and modularity.

MCU - Adafruit Huzzah32 Feather: https://www.adafruit.com/product/3405
GPS - Adafruit Ultimate GPS: https://www.adafruit.com/product/3133
RF - Adafruit RFM95W 433MHz: https://www.adafruit.com/product/3232
RTC/SD - Adafruit Adalogger: https://www.adafruit.com/product/2922

## Learning Resources
