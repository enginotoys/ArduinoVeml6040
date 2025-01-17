/*

The MIT License (MIT)

Copyright (c) 2015 thewknd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Wire.h"
#include <math.h>
#include "veml6040.h"


VEML6040::VEML6040(void) {
  
}

bool VEML6040::begin(void) {
  bool sensorExists = false;
  Wire.begin();
  Wire.beginTransmission(VEML6040_I2C_ADDRESS);
  if (Wire.endTransmission() == 0) {
    sensorExists = true;
  }
  return sensorExists;
}

void VEML6040::setConfiguration(uint8_t configuration) {
  Wire.beginTransmission(VEML6040_I2C_ADDRESS);  
  Wire.write(COMMAND_CODE_CONF); 
  Wire.write(configuration); 
  Wire.write(0);
  Wire.endTransmission(); 
  lastConfiguration = configuration;
}

uint16_t VEML6040::read(uint8_t commandCode) {
  uint16_t data = 0; 
  
  Wire.beginTransmission(VEML6040_I2C_ADDRESS);
  Wire.write(commandCode);
  Wire.endTransmission(false);
  Wire.requestFrom(VEML6040_I2C_ADDRESS,2);
  while(Wire.available()) 
  {
    data = Wire.read(); 
    data |= Wire.read() << 8;
  }
  
  return data; 
}

uint16_t VEML6040::getRed(void) {
  return(read(COMMAND_CODE_RED));
}

uint16_t VEML6040::getGreen(void) {
  return(read(COMMAND_CODE_GREEN));
}

uint16_t VEML6040::getBlue(void) {
  return(read(COMMAND_CODE_BLUE));
}

uint16_t VEML6040::getWhite(void) {
  return(read(COMMAND_CODE_WHITE));
}

float VEML6040::getAmbientLight(void) {
  uint16_t sensorValue; 
  float ambientLightInLux;
  
  sensorValue = read(COMMAND_CODE_GREEN);
  
  switch(lastConfiguration & 0x70) {
  
    case VEML6040_IT_40MS:    ambientLightInLux = sensorValue * VEML6040_GSENS_40MS;
                              break;
    case VEML6040_IT_80MS:    ambientLightInLux = sensorValue * VEML6040_GSENS_80MS;
                              break;
    case VEML6040_IT_160MS:   ambientLightInLux = sensorValue * VEML6040_GSENS_160MS;
                              break;
    case VEML6040_IT_320MS:   ambientLightInLux = sensorValue * VEML6040_GSENS_320MS;
                              break;
    case VEML6040_IT_640MS:   ambientLightInLux = sensorValue * VEML6040_GSENS_640MS;
                              break; 
    case VEML6040_IT_1280MS:  ambientLightInLux = sensorValue * VEML6040_GSENS_1280MS; 
                              break;   
    default:                  ambientLightInLux = -1;
                              break;                             
  } 
  return ambientLightInLux;
}

uint16_t VEML6040::getCCT(float offset) {
  uint16_t red,blue,green;
  float cct,ccti;
  
  red = read(COMMAND_CODE_RED);
  green = read(COMMAND_CODE_GREEN);
  blue = read(COMMAND_CODE_BLUE);
  
  ccti = ((float)red-(float)blue) / (float)green;
  ccti = ccti + offset; 
  cct = 4278.6 * pow(ccti,-1.2455);
  
  return((uint16_t)cct);
}

float VEML6040::rgb2hsv(uint8_t index) {
  static float hsv[3]; // Static so it persists after function returns
  float rf = float(read(COMMAND_CODE_RED)) / 65535.0;
  float gf = float(read(COMMAND_CODE_GREEN)) / 65535.0;
  float bf = float(read(COMMAND_CODE_BLUE)) / 65535.0;
  float high = fmax(fmax(rf, gf), bf);
  float low = fmin(fmin(rf, gf), bf);
  float h, s, v;
  float d = high - low;
  v = high;
  s = high == 0 ? 0 : d / high;
  if (high == low) {
    h = 0.0;
  } else {
    if (high == rf) {
      h = (gf - bf) / d + (gf < bf ? 6.0 : 0.0);
    } else if (high == gf) {
      h = (bf - rf) / d + 2.0;
    } else {
      h = (rf - gf) / d + 4.0;
    }
    h /= 6.0;
  }
  hsv[0] = h * 360.0; // Hue
  hsv[1] = s;         // Saturation
  hsv[2] = v;         // Value
  
  // Return the requested component based on the index parameter
  if (index == 0) {
    return hsv[0];   // Hue
  } else if (index == 1) {
    return hsv[1];   // Saturation
  } else if (index == 2) {
    return hsv[2];   // Value
  } else {
    // Handle invalid index; in this case, return hue as default
    return hsv[0];
  }
}
