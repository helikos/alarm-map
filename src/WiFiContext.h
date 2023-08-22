#pragma once
#ifndef _WIFI_CONTEXT_H
#define _WIFI_CONTEXT_H

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <WiFi.h>


class WiFiContext {

  private:
    static TimerHandle_t _wifiReconnectTimer;
    static TimerHandle_t _flashTimer;
    static bool flashFlag;

    static const char* _ssid;
    static const char* _pass;
    static const char* describeWiFiEvent(WiFiEvent_t event);
    static uint32_t lastColor;
    static void stopTimer();
    static void startTimer();

    static void setGreenPixel();
    static void setColorPixel();
    static void setNonePixel();

  public:
    static void connectToWifi();
    static void flashLed();

    static void WiFiEvent(WiFiEvent_t event);
    static void initializate(const char* ssid, const char* pass);

};

#endif // _WIFI_CONTEXT_H
