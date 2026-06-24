#include <Arduino.h>
#include <U8x8lib.h>

#include "RolladenCtrl.h"
#include "Display.h"
#include "OpAuto.h"
#include "TimeLib.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

/*
  Based on...
  
  "Hello World" version for U8x8 API

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

/* 
connection
  VIN - red
  GND - blue
  A5 - SCK - green
  A4 - SDA - black
*/

// Please UNCOMMENT one of the contructor lines below
// U8x8 Contructor List 
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
//U8X8_NULL u8x8;  // null device, a 8x8 pixel display which does nothing
//U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 6, /* dc=*/ 4, /* reset=*/ 12);  // Arduboy (DevKit)
//U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6);  // Arduboy 10 (Production, Kickstarter Edition)
//U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_3W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_3W_HW_SPI u8x8(/* cs=*/ 10, /* reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);        
//U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);        // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 2, /* data=*/ 0, /* reset=*/ U8X8_PIN_NONE);         // Digispark ATTiny85
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
//U8X8_SSD1306_128X64_VCOMH0_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);    // same as the NONAME variant, but maximizes setContrast() range
//U8X8_SSD1306_128X64_ALT0_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);    // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8X8_SSD1306_102X64_EA_OLEDS102_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);   // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8X8_SSD1312_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1312_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ 8);
//U8X8_SSD1312_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

//U8X8_SH1106_128X64_VCOMH0_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);   // same as the NONAME variant, but maximizes setContrast() range
//U8X8_SH1106_128X64_WINSTAR_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);    // same as the NONAME variant, but uses updated SH1106 init sequence
//U8X8_SH1106_128X32_VISIONOX_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 
//U8X8_SH1106_128X32_VISIONOX_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SH1106_72X40_WISE_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SH1107_64X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SH1107_128X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SH1107_128X128_HW_I2C u8x8(/* reset=*/ 8);
//U8X8_SH1107_128X80_HW_I2C u8x8(/* reset=*/ 8);
//U8X8_SH1107_PIMORONI_128X128_HW_I2C u8x8(/* reset=*/ 8);
//U8X8_SH1107_SEEED_128X128_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1107_SEEED_128X128_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1107_SEEED_96X96_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SH1108_160X160_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//U8X8_SSD1306_128X32_WINSTAR_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//U8X8_SSD1306_64X48_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.66" OLED breakout board, Uno: A4=SDA, A5=SCL, 5V powered
//U8X8_SSD1306_48X64_WINSTAR_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_64X32_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);  
//U8X8_SSD1306_64X32_1F_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);  
//U8X8_SSD1306_96X16_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.69"
//U8X8_SSD1306_72X40_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42"
//U8X8_SSD1306_96X40_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42"
//U8X8_SSD1306_128X64_NONAME_6800 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_8080 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1309_128X64_NONAME0_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1309_128X64_NONAME2_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1309_128X64_NONAME2_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1316_128X32_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1316_128X32_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1317_96X96_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // not tested, not confirmed
//U8X8_SSD1317_96X96_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // not tested, not confirmed
//U8X8_SSD1318_128X96_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1318_128X96_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1320_160X132_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1320_160X132_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1322_NHD_256X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1322_NHD_256X64_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1322_NHD_128X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1322_NHD_128X64_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1325_NHD_128X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1325_NHD_128X64_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_SSD0323_OS128064_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD0323_OS128064_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_SSD1326_ER_256X32_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);         // experimental driver for ER-OLED018-1
//U8X8_SSD1327_SEEED_96X96_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); // Seeedstudio Grove OLED 96x96
//U8X8_SSD1327_SEEED_96X96_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);  // Seeedstudio Grove OLED 96x96
//U8X8_SSD1327_EA_W128128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_EA_W128128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_MIDAS_128X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_MIDAS_128X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_WS_128X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_WS_128X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_VISIONOX_128X96_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1327_VISIONOX_128X96_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1329_128X96_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1329_128X96_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_SSD1329_96X96_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1329_96X96_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_SSD1329_96X96_NONAME_4W_HW_SPI u8x8(/* cs=*/ A4, /* dc=*/ A2, /* reset=*/ U8X8_PIN_NONE); //  Adafruit Feather 32u4 Basic Proto
//U8X8_SSD1305_128X32_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X32_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X32_ADAFRUIT_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X32_ADAFRUIT_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X64_ADAFRUIT_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X64_ADAFRUIT_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X64_RAYSTAR_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1305_128X64_RAYSTAR_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_KS0108_128X64 u8x8(8, 9, 10, 11, 4, 5, 6, 7, /*enable=*/ 18, /*dc=*/ 17, /*cs0=*/ 14, /*cs1=*/ 15, /*cs2=*/ U8X8_PIN_NONE, /* reset=*/  U8X8_PIN_NONE);  // Set R/W to low!
//U8X8_KS0108_ERM19264 u8x8(8, 9, 10, 11, 4, 5, 6, 7, /*enable=*/ 18, /*dc=*/ 17, /*cs0=*/ 14, /*cs1=*/ 15, /*cs2=*/ 16, /* reset=*/  U8X8_PIN_NONE);   // Set R/W to low!
//U8X8_HD44102_100X64 u8x8(4, 5, 6, 7, 8, 9, 10, 11, /*enable=*/ 2, /*dc=*/ 3, /*cs0=*/ A0, /*cs1=*/ A1, /*cs2=*/ A2, /* reset=*/  U8X8_PIN_NONE);  // Set R/W to low!
//U8X8_T7932_150X32 u8x8(4, 5, 6, 7, 8, 9, 10, 11, /*enable=*/ 2, /*dc=*/ 3, /*cs0=*/ A0, /*cs1=*/ A1, /*cs2=*/ A2, /* reset=*/  U8X8_PIN_NONE);  // Set R/W to low!
//U8X8_UC1701_EA_DOGS102_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1701_EA_DOGS102_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_PCD8544_84X48_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // Nokia 5110 Display
//U8X8_PCD8544_84X48_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);              // Nokia 5110 Display
//U8X8_PCF8812_96X65_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // Could be also PCF8814
//U8X8_PCF8812_96X65_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);      // Could be also PCF8814
//U8X8_HX1230_96X68_3W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8X8_HX1230_96X68_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7528_ERC16064_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7528_ERC16064_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_EA_DOGM128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_EA_DOGM128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_64128N_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_64128N_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_EA_DOGM132_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ U8X8_PIN_NONE); // DOGM132 Shield
//U8X8_ST7565_EA_DOGM132_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ U8X8_PIN_NONE);  // DOGM132 Shield
//U8X8_ST7565_ZOLEN_128X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_ZOLEN_128X64_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_LM6059_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // Adafruit ST7565 GLCD
//U8X8_ST7565_LM6059_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);    // Adafruit ST7565 GLCD
//U8X8_ST7565_KS0713_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // KS0713 controller
//U8X8_ST7565_KS0713_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);    // KS0713 controller
//U8X8_ST7565_LX12864_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_ST7565_LX12864_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);   
//U8X8_ST7565_ERC12864_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_ERC12864_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_ERC12864_ALT_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // contrast improved version for ERC12864
//U8X8_ST7565_ERC12864_ALT_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // contrast improved version for ERC12864
//U8X8_ST7565_NHD_C12832_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_NHD_C12832_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_NHD_C12864_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_NHD_C12864_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_JLX12864_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7565_JLX12864_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7567_PI_132X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  // Pax Instruments Shield, LCD_BL=6
//U8X8_ST7567_PI_132X64_4W_HW_SPI u8x8(/* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  // Pax Instruments Shield, LCD_BL=6
//U8X8_ST7567_JLX12864_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_ST7567_JLX12864_4W_HW_SPI u8x8(/* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_122X32_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_ST7567_122X32_4W_HW_SPI u8x8(/* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_OS12864_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_ST7567_OS12864_4W_HW_SPI u8x8(/* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_ENH_DG128064_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_ENH_DG128064_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_ENH_DG128064I_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_ENH_DG128064I_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_ST7567_64X32_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 
//U8X8_ST7567_HEM6432_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 
//U8X8_ST7571_128X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7571_128X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7571_128X96_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST7571_128X96_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75160_JM16096_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75160_JM16096_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX172104_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX172104_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX19296_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX19296_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256128_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_WO256X128_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_WO256X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256160_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256160_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256160M_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256160M_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256160_ALT_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX256160_ALT_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX240160_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX240160_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75256_JLX25664_2ND_HW_I2C u8x8(/* reset=*/ 8); // Due, 2nd I2C
//U8X8_ST75320_JLX320240_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_ST75320_JLX320240_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_NT7534_TG12864R_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_NT7534_TG12864R_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_ST7588_JLX12864_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ 5);  
//U8X8_ST7588_JLX12864_HW_I2C u8x8(/* reset=*/ 5);
//U8X8_IST3020_ERC19264_6800 u8x8(44, 43, 42, 41, 40, 39, 38, 37,  /*enable=*/ 28, /*cs=*/ 32, /*dc=*/ 30, /*reset=*/ 31); // Connect WR pin with GND
//U8X8_IST3020_ERC19264_8080 u8x8(44, 43, 42, 41, 40, 39, 38, 37,  /*enable=*/ 29, /*cs=*/ 32, /*dc=*/ 30, /*reset=*/ 31); // Connect RD pin with 3.3V
//U8X8_IST3020_ERC19264_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_IST7920_128X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // Round display
//U8X8_IST7920_128X128_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // Round display
//U8X8_UC1601_128X32_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_UC1601_128X32_3W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8X8_UC1601_128X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_UC1604_JLX19264_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_UC1604_JLX19264_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_UC1609_SLG19264_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_UC1609_SLG19264_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  
//U8X8_UC1608_ERC24064_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // Due, SW SPI, ERC24064-1 Test Board 
//U8X8_UC1608_DEM240064_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // Due, SW SPI, ERC24064-1 Test Board 
//U8X8_UC1608_ERC240120_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1608_240X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_UC1610_EA_DOGXL160_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/  U8X8_PIN_NONE);
//U8X8_UC1610_EA_DOGXL160_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/  U8X8_PIN_NONE);
//U8X8_UC1611_EA_DOGM240_2ND_HW_I2C u8x8(/* reset=*/ 8);  // Due, 2nd I2C, DOGM240 Test Board
//U8X8_UC1611_EA_DOGM240_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // SW SPI, Due DOGXL240 Test Board
//U8X8_UC1611_EA_DOGXL240_2ND_HW_I2C u8x8(/* reset=*/ 8); // Due, 2nd I2C, DOGXL240 Test Board
//U8X8_UC1611_EA_DOGXL240_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // SW SPI, Due DOGXL240 Test Board
//U8X8_UC1611_IDS4073_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8X8_UC1611_IDS4073_4W_HW_SPI u8x8( /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1611_EW50850_8080 u8x8(8, 9, 10, 11, 4, 5, 6, 7,  /*enable=*/ 18, /*cs=*/ 3, /*dc=*/ 16, /*reset=*/ 17); // 240x160, Connect RD/WR1 pin with 3.3V, CS is aktive high
//U8X8_UC1611_CG160160_8080 u8x8(8, 9, 10, 11, 4, 5, 6, 7,  /*enable=*/ 18, /*cs=*/ 3, /*dc=*/ 16, /*reset=*/ 17); // Connect WR1 and CD1 pin with 3.3V, connect CS0 with cs, WR0 with enable, CD with dc
//U8X8_UC1617_JLX128128_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1617_JLX128128_4W_HW_SPI u8x8( /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1638_192X96_4W_SW_SPI u8x8( /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1638_192X96_4W_HW_SPI u8x8( /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_UC1638_192X96_SW_I2C u8x8(/* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);  // u8g2 test board: I2C clock/data is same as SPI, I2C default address is 0x78
//U8X8_SSD1606_172X72_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // eInk/ePaper Display
//U8X8_SSD1607_200X200_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // eInk/ePaper Display, original LUT from embedded artists
//U8X8_SSD1607_GS_200X200_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // Good Display
//U8X8_SSD1607_WS_200X200_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // Waveshare
//U8X8_IL3820_296X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // eInk/ePaper Display, original LUT
//U8X8_IL3820_V2_296X128_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); // eInk/ePaper Display, optimized (lesser screen flicker)
//U8X8_ST7511_AVD_320X240_8080 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable/WR=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8); // Connect RD pin with 3.3V/5V
//U8X8_S1D15721_240X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_S1D15721_240X64_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_GU800_128X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_GU800_128X64_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// End of constructor list



// Wohnzimmer links
/*
.... .xx.
.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... xxxx

.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... xxxx
.... .xx.
*/
const uint8_t win1_B1[8] = { 0,0, 0,0, 254,255, 255,254 };
const uint8_t win1_B2[8] = { 0,0, 0,0, 127,255, 255,127 };

/*
.... .xx.
.... x..x
.... x..x
.... x..x
.... x..x
.... x..x
.... x..x
.... x..x

.... x..x
.... x..x
.... x..x
.... x..x
.... x..x
.... x..x
.... x..x
.... .xx.
*/
const uint8_t win1_L1[8] = { 0,0, 0,0, 254,1, 1,254 };
const uint8_t win1_L2[8] = { 0,0, 0,0, 127,128, 128,127 };


// Erker links
/*
          .    .
          .... .xx.
          .... xxxx
          ...x xxxx
          ..xx xxx.

.... .... .xxx xx..
.... .... xxxx x...
.... ...x xxxx ....
.... ..xx xxx. ....
.... .xxx xx.. ....
.... xxxx x... ....
...x xxxx .... ....
..xx xxx. .... ....

.xxx xx.. 
xxxx x... 
xxxx .... 
.xx. ....
.    .
*/
const uint8_t win2_B1[8] = { 0,0, 128,192, 224,240, 240,96 };
const uint8_t win2_B2[16] = { 0,0, 128,192, 224,240, 248,124,   62,31, 15,7, 3,1, 0,0 };
const uint8_t win2_B4[8] = { 6,15, 15,7, 3,1, 0,0 };

/*
          .    .
          .... .xx.
          .... x..x
          ...x ...x
          ..x. ..x.

.... .... .x.. .x..
.... .... x... x...
.... ...x ...x ....
.... ..x. ..x. ....
.... .x.. .x.. ....
.... x... x... ....
...x ...x .... ....
..x. ..x. .... ....

.x.. .x.. 
x... x... 
x..x .... 
.xx. ....
.    .
*/
const uint8_t win2_L1[8] = { 0,0, 128,64, 32,16, 144,96 };
const uint8_t win2_L2[16] = { 0,0, 128,64, 32,16, 136,68,   34,17, 8,4, 2,1, 0,0 };
const uint8_t win2_L4[8] = { 6,9, 8,4, 2,1, 0,0 };


// Erker mitte
/*
.... .xxx xxxx xxxx xxxx xxx.
.... xxxx xxxx xxxx xxxx xxxx
.... xxxx xxxx xxxx xxxx xxxx
.... .xxx xxxx xxxx xxxx xxx.
.    .    .    .    .    .
*/
const uint8_t win3_B1[24] = { 0,0, 0,0, 6,15, 15,15,   15,15, 15,15, 15,15, 15,15,   15,15, 15,15, 15,15, 15,6 };

/*
.... .xxx xxxx xxxx xxxx xxx.
.... x... .... .... .... ...x
.... x... .... .... .... ...x
.... .xxx xxxx xxxx xxxx xxx.
.    .    .    .    .    .
*/
const uint8_t win3_L1[24] = { 0,0, 0,0, 6,9, 9,9,   9,9, 9,9, 9,9, 9,9,   9,9, 9,9, 9,9, 9,6 };


// Erker rechts/Terrassentuer
/*
.    .    .    .
.... .xx. .... ....
.... xxxx .... ....
.... xxxx x... ....
.... .xxx xx.. ....

.... ..xx xxx. ....
.... ...x xxxx ....
.... .... xxxx x...
.... .... .xx. xx..
.... .... ..xx xxx.
.... .... ...x xxxx
.... .... .... xxxx
.... .... .... .xx.
*/
const uint8_t win4_B1[16] = { 0,0, 0,0, 96,240, 240,224,   192,128, 0,0, 0,0, 0,0 };
const uint8_t win4_B3[16] = { 0,0, 0,0, 0,0, 1,3,   7,15, 31,62, 124,248, 240,96 };

/*
.    .    .    .
.... .xx. .... ....
.... x..x .... ....
.... x... x... ....
.... .x.. .x.. ....

.... ..x. ..x. ....
.... ...x ...x ....
.... .... x... x...
.... .... .x.. .x..
.... .... ..x. ..x.
.... .... ...x ...x
.... .... .... x..x
.... .... .... .xx.
*/
const uint8_t win4_L1[16] = { 0,0, 0,0, 96,144, 16,32,   64,128, 0,0, 0,0, 0,0 };
const uint8_t win4_L3[16] = { 0,0, 0,0, 0,0, 1,2,   4,8, 17,34, 68,136, 144,96 };


// Wohnzimmer rechts
/*
.... .xxx xxxx xxx.
.... xxxx xxxx xxxx
.... xxxx xxxx xxxx
.... .xxx xxxx xxx.
.    .    .    .
*/
const uint8_t win5_B1[16] = { 0,0, 0,0, 6,15, 15,15,   15,15, 15,15, 15,15, 15,6 };

/*
.... .xxx xxxx xxx.
.... x... .... ...x
.... x... .... ...x
.... .xxx xxxx xxx.
.    .    .    .
*/
const uint8_t win5_L1[16] = { 0,0, 0,0, 6,9, 9,9,   9,9, 9,9, 9,9, 9,6 };


// Kueche links
/*
.xxx xxxx xxxx xxxx xxx. ....
xxxx xxxx xxxx xxxx xxxx ....
xxxx xxxx xxxx xxxx xxxx ....
.xxx xxxx xxxx xxxx xxx. ....
.    .    .    .    .    .
*/
const uint8_t win6_B1[24] = { 6,15, 15,15, 15,15, 15,15,   15,15, 15,15, 15,15, 15,15,   15,15, 15,6, 0,0, 0,0};

/*
.xxx xxxx xxxx xxxx xxx. ....
x... .... .... .... ...x ....
x... .... .... .... ...x ....
.xxx xxxx xxxx xxxx xxx. ....
.    .    .    .    .    .
*/
const uint8_t win6_L1[24] = { 6,9, 9,9, 9,9, 9,9,   9,9, 9,9, 9,9, 9,9,   9,9, 9,6, 0,0, 0,0 };


// Kueche rechts, Terrassentuer
/*
.xxx xxxx xxxx xxx.
xxxx xxxx xxxx xxxx
xxxx xxxx xxxx xxxx
.xxx xxxx xxxx xxx.
.    .    .    .
*/
const uint8_t win7_B1[16] = { 6,15, 15,15, 15,15, 15,15,   15,15, 15,15, 15,15, 15,6 };

/*
.xxx xxxx xxxx xxx.
x... .... .... ...x
x... .... .... ...x
.xxx xxxx xxxx xxx.
.    .    .    .
*/
const uint8_t win7_L1[16] = { 6,9, 9,9, 9,9, 9,9,   9,9, 9,9, 9,9, 9,6 };

/*
empty dummy - 3chars wide
*/
const uint8_t win_empty[24] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };


/*
typedef struct {
  union {
    const uint8_t *raster_light, *raster_bold;
    const uint8_t *raster[2];
  };
} RASTER_T;
*/
typedef struct {
  const uint8_t x,y;  // display position
  const uint8_t c;  // character length, 8bytes each char
  const uint8_t *raster[2];  // light,bold
} TILE_T;

const TILE_T tileWin[7][3] =  // 7 windows, selected, 3 tiles
{
// Wohnzimmer links
  {
    { 1,6,1, {win1_L1, win1_B1} },  // light/unselected, bold/selected
    { 1,7,1, {win1_L2, win1_B2} },
    { 0,0,0, {NULL, NULL} }
  },
// Erker links
  {
    { 1,0,1, {win2_L1, win2_B1} },
    { 0,1,2, {win2_L2, win2_B2} },
    { 0,2,1, {win2_L4, win2_B4} }
  },
// Erker mitte
  {
    { 2,0,3, {win3_L1, win3_B1} },
    { 0,0,0, {NULL, NULL} },
    { 0,0,0, {NULL, NULL} }
  },
// Erker rechts/Terrassentuer
  {
    { 5,0,2, {win4_L1, win4_B1} },
    { 5,1,2, {win4_L3, win4_B3} },
    { 0,0,0, {NULL, NULL} }
  },
// Wohnzimmer rechts
  {
    { 7,2,2, {win5_L1, win5_B1} },
    { 0,0,0, {NULL, NULL} },
    { 0,0,0, {NULL, NULL} }
  },
// Kueche links
  {
    { 11,2,3, {win6_L1, win6_B1} },
    { 0,0,0, {NULL, NULL} },
    { 0,0,0, {NULL, NULL} }
  },
// Kueche rechts/Terrassentuer
  {
    { 14,2,2, {win7_L1, win7_B1} },
    { 0,0,0, {NULL, NULL} },
    { 0,0,0, {NULL, NULL} }
  },
};



CDisplay::CDisplay(void)
{
  u8x8.begin();
  u8x8.setPowerSave(0);

  u8x8.setFlipMode(true);
  u8x8.clear();
}

void CDisplay::clear(void)
{
  u8x8.clearDisplay();
}

#include "Dprintf.h"

void CDisplay::displayWindows(uint8_t bf_selected, uint8_t bf_heartbeat, uint8_t bf_flash, uint8_t bf_show)
{
  // display shade selection
  uint8_t i, c;
  TILE_T *tile;
  uint8_t *raster;

  for(i=0; i<7; i++)  // windows
  {
    for(c=0; c<3; c++)  // max 3 tiles per icon
    {
      tile = (TILE_T*) &(tileWin[i][c]);  // 7 windows, up to 3 tiles each
      raster = (uint8_t*) tile->raster[bitRead(bf_selected, i)];  // unselected/selected
      if(tile->c != 0)  // tile available -> print
      {
        if(bitRead(bf_heartbeat, i))  // active - flash light/bold
        {
          if(bitRead(millis(), 9) /*about 500ms*/)
          {
            raster = (uint8_t*) tile->raster[1-bitRead(bf_selected, i)];  // invert light/bold
          }          
        }
        else if(bitRead(bf_flash, i))  // flash: show/no show
        {
          if(bitRead(millis(), 9) /*about 500ms*/)
          {
            raster = (uint8_t*) win_empty;
          }          
        }
        else if(!bitRead(bf_show, i))  // no show
        {
          raster = (uint8_t*) win_empty;  // print... but no show/clear
        }

        if(raster != NULL)  // shouldn't happen... just to make sure
        {
          u8x8.drawTile(tile->x,tile->y, tile->c, raster);          
        }
      }
    }  // 3 tiles
  }  // 7 shades
}


void CDisplay::displayTime(bool heartbeat)
{
  char str[16];

  // date
  snprintf(str, 16, "%02u.%02u.%04u", day(), month(), year());
  //u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.setFont(u8x8_font_artossans8_n);
  u8x8.drawString(6,4, str);

  // time
  snprintf(str, 16, "%2u%c%02u", hour(), heartbeat?':':' ', minute());
  u8x8.setFont(u8x8_font_courB18_2x3_n);
  u8x8.drawString(6,5, str);
}


// debug something
void CDisplay::displayState(uint8_t state, bool buttonUp, bool buttonDown)
{
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 
  char str[9];
  snprintf(str,9, "    ");  // empty
  switch(state)  // system state
  {
    case STATE_IDLE: /*snprintf(str,9, "Idle");*/ break;
    case STATE_MOVING: snprintf(str,9, "Move"); break;
    case STATE_REINIT: snprintf(str,9, "Init"); break;
    default: /*snprintf(str,9, "undf")*/ break;
  }
  u8x8.drawString(12,3, str);  // 4char state

  if(buttonUp || buttonDown)
    snprintf(str,7, "%s%s      ", buttonUp?"Up":"", buttonDown?"Down":"");
  else
    snprintf(str,7, "      ");
  u8x8.drawString(6,3, str);  // 6char UpDown

  snprintf(str,9, "%08lx", millis());  // debug: internal time
  u8x8.drawString(8,0, str);
}


void CDisplay::displayProgram(uint8_t prgm, uint8_t actUI, uint8_t actUI_flash)
{
  char str[16];

  // actUI=0 - nothing highlighted

  // shown program id
  u8x8.setInverseFont(actUI==0);
  snprintf(str,16, "-%1u-", prgm+1);
  u8x8.setFont(u8x8_font_7x14_1x2_r);
  u8x8.drawString(13,0, str);

  // windows (actUI=1..7)
  uint8_t bf_flash = (actUI>=1 && actUI<=7)?1<<(actUI-1):0b00000000;  // actUI vs SHADECNT
  displayWindows(opAutoConfig[prgm].shadesSelected, 0b00000000, bf_flash, 0b01111111);

  // shade position  // open - % - clsd
  u8x8.setInverseFont(actUI==8 && !(actUI_flash && bitRead(millis(), 9) /*about 500ms*/));
  if(opAutoConfig[prgm].position==0)
  {
    snprintf(str,16, "open");
  }
  else
  {
    snprintf(str,16, "%3u%%", opAutoConfig[prgm].position);
  }
  u8x8.setFont(u8x8_font_7x14_1x2_r);
  u8x8.drawString(6,3, str);

  // enable
  u8x8.setInverseFont(actUI==9);
  /*
  u8x8.setFont(u8x8_font_open_iconic_check_2x2);
  u8x8.drawGlyph(4,3, opAutoConfig[prgm].enable?67 /.*tickmark*./:68 /.*disabled/cross*./);
  */
  snprintf(str,16, opAutoConfig[prgm].enable?"auto":"off ");
  u8x8.setFont(u8x8_font_7x14_1x2_r);
  u8x8.drawString(12,3, str);


  // time
  u8x8.setInverseFont(actUI==10 && !(actUI_flash && bitRead(millis(), 9) /*about 500ms*/));
  if( opAutoConfig[prgm].fix )
  {
    snprintf(str, 16, "%2u%c%02u", opAutoConfig[prgm].hour, ':', opAutoConfig[prgm].minute);
    u8x8.setFont(u8x8_font_courB18_2x3_n);
    u8x8.drawString(6,5, str);
  }
  else
  {
    u8x8.setFont(u8x8_font_open_iconic_weather_2x2);
    u8x8.drawGlyph(6,5, opAutoConfig[prgm].sunrise?69 /*sun*/:66 /*moon*/);

    u8x8.setFont(u8x8_font_artossans8_n);
    u8x8.drawString(6,7, "  \n");  // clear possible remainder from first digit hour

    if(opAutoConfig[prgm].minuteOffset==0)
    {
      snprintf(str, 16, " :00");
    }
    else
    {
      snprintf(str, 16, " %+02i", opAutoConfig[prgm].minuteOffset);
    }
    u8x8.setFont(u8x8_font_courB18_2x3_n);
    u8x8.drawString(8,5, str);
  }

  u8x8.setInverseFont(false);
}


/*
void displayRefresh()
{
  u8x8.refreshDisplay();    // only required for SSD1606/7  
}
*/
