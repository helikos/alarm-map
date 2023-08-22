

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "configuration.h"
#include "Logger.h"

extern Adafruit_NeoPixel strip;
extern uint32_t bluePixel;
extern uint32_t yellowPixel;
extern uint32_t nonePixel;

extern uint8_t ledColorBlue[];
extern uint8_t ledColorYellow[];
extern uint8_t lenghtYellowArray;
extern uint8_t lenghtBlueArray;
extern Logger logger;

void initStrip()
{
  strip.begin();                      // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                       // Turn OFF all pixels ASAP
  strip.setBrightness(ledBrightness); // встановлення яскравості (max = 255)
}

void noneStrip()
{
  logger.log(PSTR("commontFunction::noneStrip"));
  for (int i = 0; i < LED_COUNT; i++)
  {                                    // For each pixel in strip...
    strip.setPixelColor(i, nonePixel); //  Set pixel's color (in RAM)
  }
  strip.show(); //  Update strip to match
}

void colorWipe()
{
  logger.log(PSTR("commontFunction::colorWipe"));
  for (int i = 0; i < lenghtYellowArray; i++)
  {                                                      // For each pixel in strip...
    strip.setPixelColor(ledColorYellow[i], yellowPixel); //  Set pixel's color (in RAM)
  }
  for (int i = 0; i < lenghtBlueArray; i++)
  { // For each pixel in strip...
    strip.setPixelColor(ledColorBlue[i], bluePixel);
  }
  strip.show(); //  Update strip to match
}

void testMap()
{
  logger.log(PSTR("commontFunction::testMap"));
  for (int i = 0; i < LED_COUNT; i++)
  { // For each pixel in strip...
    strip.setPixelColor(i, bluePixel);
    strip.show(); //  Update strip to match
    delay(500);   //  Pause for a moment
  }

  for (int i = 0; i < LED_COUNT; i++)
  { // For each pixel in strip...
    strip.setPixelColor(i, nonePixel);
  }
  strip.show(); //  Update strip to match
  delay(500);   //  Pause for a moment
}

void getFreeHeapSize(AsyncWebServerRequest *request)
{
  String status = "Hi! I am alarm map.\n";
  status += "Free heap memory size is ";
  status += String(esp_get_free_heap_size());
  status += " bytes \n";
  request->send(200, "text/plain", status);
}
