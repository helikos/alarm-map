#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <limits.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "configuration.h"
#include "secret.h"

#include "WiFiContext.h"

#include "Logger.h"
#include "commonFunctions.h"

WiFiContext wifiContext;
Logger logger;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);

uint8_t ledColorBlue[] = {4, 5, 6, 7, 20, 8, 9, 19, 10, 11, 25};
uint8_t ledColorYellow[] = {0, 1, 2, 3, 24, 23, 22, 21, 16, 17, 18, 14, 15, 13, 12};
uint8_t lenghtYellowArray = sizeof(ledColorYellow) / sizeof(ledColorYellow[0]);
uint8_t lenghtBlueArray = sizeof(ledColorBlue) / sizeof(ledColorBlue[0]);

uint32_t nonePixel = strip.Color(0, 0, 0);
uint32_t redPixel = strip.Color(201, 0, 0);
uint32_t greenPixel = strip.Color(50, 174, 0);
uint32_t bluePixel = strip.Color(0, 56, 255); // strip.Color(  0, 0,   255))
uint32_t whitePixel = strip.Color(255, 255, 255);
uint32_t yellowPixel = strip.Color(253, 166, 0); // strip.Color(  255, 255,  0)
uint32_t c1Pixel = strip.Color(255, 55, 0);
uint32_t c2Pixel = strip.Color(0, 255, 0);

static String states[] = {"Закарпатська область",
                          "Івано-Франківська область", "Тернопільська область", "Львівська область", "Волинська область", "Рівненська область",
                          "Житомирська область", "Київська область", "Чернігівська область", "Сумська область", "Харківська область", "Луганська область",
                          "Донецька область", "Запорізька область", "Херсонська область", "АР Крим", "Одеська область", "Миколаївська область", "Дніпропетровська область",
                          "Полтавська область", "Черкаська область", "Кіровоградська область", "Вінницька область", "Хмельницька область", "Чернівецька область", "м. Київ"};

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long timerDelay = 10000;
unsigned long lastTime = millis() + timerDelay;
unsigned long hv = 180000;

DynamicJsonDocument doc(30000);

static unsigned long times[LED_COUNT];
static uint32_t ledColor[LED_COUNT];

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.println("__ OK __");
  logger.logInitialization();
  initStrip();
  colorWipe();
  delay(2000);
  wifiContext.initializate(_SSID, _PASS);

  server.on("/log", HTTP_GET, Logger::publishLog);
  server.on("/memory", HTTP_GET, getFreeHeapSize);
  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  Serial.println("Setup Ended");
  Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
}

void loop()
{

  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      logger.log(PSTR("Start Http"));
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      auto httpInitResult = http.begin(baseURL);
      String response;
      if (httpInitResult == false)
      {
        logger.log(PSTR("http.begin() failed "));
      }
      else
      {
        logger.log(PSTR("Http get"));
        int httpCode = http.GET();
        if (httpCode == 200)
        { // Check for the returning code
          response = http.getString();
          DeserializationError error = deserializeJson(doc, response);
          unsigned long callTime = millis();
          for (int i = 0; i < LED_COUNT; i++)
          {
            bool enable = doc["states"][states[i]]["enabled"].as<bool>();
            if (enable && times[i] == 0)
            {
              times[i] = callTime;
              ledColor[i] = c1Pixel;
            }
            else if (enable && times[i] + hv > callTime && ledColor[i] != redPixel)
            {
              ledColor[i] = c1Pixel;
            }
            else if (enable)
            {
              ledColor[i] = redPixel;
              times[i] = callTime;
            }
            if (!enable && times[i] + hv > callTime && times[i] != 0)
            {
              ledColor[i] = c2Pixel;
            }
            else if (!enable)
            {
              ledColor[i] = c2Pixel;
              times[i] = 0;
            }
          }
          for (int i = 0; i < LED_COUNT; i++)
          {
            strip.setPixelColor(i, ledColor[i]);
          }
          strip.show();
        }
        else
        {
          logger.log(PSTR("Error on HTTP request"));
        }
      }
      // Free resources
      http.end();
    }
    else
    {
      logger.log(PSTR("WiFi Disconnected"));
    }
    lastTime = millis();
  }
}
