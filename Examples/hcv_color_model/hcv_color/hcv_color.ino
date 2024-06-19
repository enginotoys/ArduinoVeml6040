#include <Arduino.h>
#include "Wire.h"
#include "veml6040.h"

VEML6040 RGBWSensor; // Assuming you have a global instance of VEML6040

void setup() {
    Serial.begin(9600);
    Wire.begin(); 
    if (!RGBWSensor.begin()) {
        Serial.println("ERROR: couldn't detect the sensor");
        while(1) {} // Hang here if sensor initialization fails
    }
    RGBWSensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
  
    delay(3000); // Delay for sensor initialization
}

void loop() {
float hueValue = RGBWSensor.rgb2hsv(0);
float saturationValue = RGBWSensor.rgb2hsv(1);
float valueValue = RGBWSensor.rgb2hsv(2);

Serial.print("Hue: ");
Serial.println(hueValue);
Serial.print("Saturation: ");
Serial.println(saturationValue);
Serial.print("Value: ");
Serial.println(valueValue);
delay(100); // Delay before reading again
}
