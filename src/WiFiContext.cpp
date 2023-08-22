
#include <WiFi.h>
#include "WiFiContext.h"
#include <Adafruit_NeoPixel.h>
#include "Logger.h"
#include "commonFunctions.h"

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#define wifiTimeOfReconect pdMS_TO_TICKS(10000)
#define wifiFlashTimer pdMS_TO_TICKS(1000)

TimerHandle_t WiFiContext::_wifiReconnectTimer = NULL;
TimerHandle_t WiFiContext::_flashTimer;

const char *WiFiContext::_ssid = NULL;
const char *WiFiContext::_pass = NULL;
bool WiFiContext::flashFlag = true;

extern Logger logger;
extern uint32_t redPixel;
extern uint32_t greenPixel;
extern uint32_t nonePixel;
extern Adafruit_NeoPixel pixels;


void WiFiContext::initializate(const char *ssid, const char *pass)
{
    logger.log(PSTR("WiFiContext::initializate started"));
    _wifiReconnectTimer = xTimerCreate(PSTR("wifiTimer"), wifiTimeOfReconect, pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
    _flashTimer = xTimerCreate(PSTR("wifiFlashTimer"), wifiFlashTimer, pdTRUE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(flashLed));

    _ssid = ssid;
    _pass = pass;
    flashFlag = true;
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(WiFiEvent);
    connectToWifi();
    logger.log(PSTR("WiFiContext::initializate completed"));
}

void WiFiContext::setGreenPixel() {
    colorWipe();
}

void WiFiContext::setColorPixel() {
    colorWipe();
}

void WiFiContext::setNonePixel() {
    noneStrip();
}

void WiFiContext::flashLed() {
    logger.log(PSTR("WiFiContext::flashLed"));
    if (flashFlag) {
        WiFiContext::setColorPixel();
    } else {
        WiFiContext::setNonePixel();
    }
    flashFlag = !flashFlag;
}


void WiFiContext::connectToWifi()
{
    logger.log(PSTR("Connecting to Wi-Fi..."));
    WiFi.begin(_ssid, _pass);
    Serial.print("Connecting to WiFi ..");
    delay(1000);
}
 
void WiFiContext::stopTimer() {
    if (xTimerIsTimerActive(_wifiReconnectTimer) == pdTRUE) {
        logger.log(PSTR("WiFiContext::stopTimer _wifiReconnectTimer"));
        xTimerStop(_wifiReconnectTimer, 0);
    }
    if (xTimerIsTimerActive(_flashTimer) == pdTRUE) {
        logger.log(PSTR("WiFiContext::stopTimer _flashTimer"));
        xTimerStop(_flashTimer, 0);
    }
}

void WiFiContext::startTimer() {
    if (xTimerIsTimerActive(_wifiReconnectTimer) == pdFALSE) {
        logger.log(PSTR("WiFiContext::startTimer _wifiReconnectTimer"));
        xTimerStart(_wifiReconnectTimer, 0);
    }
    if (xTimerIsTimerActive(_flashTimer) == pdFALSE) {
        logger.log(PSTR("WiFiContext::startTimer _flashTimer"));
        xTimerStart(_flashTimer, 0);
    }
}

void WiFiContext::WiFiEvent(WiFiEvent_t event)
{
    logger.log(PSTR("WiFiContext::WiFiEvent: %s"), WiFiContext::describeWiFiEvent(event));
    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi connected"));                            //  "was connnected to WiFi"
        logger.log(PSTR("WiFiContext::WiFiEvent IP address: %s"), WiFi.localIP().toString()); //  "IP: "
        WiFiContext::stopTimer();
        WiFiContext::setGreenPixel();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi lost connection"));
        WiFiContext::startTimer();
        break;
    case SYSTEM_EVENT_WIFI_READY:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi interface ready"));
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        logger.log(PSTR("WiFiContext::WiFiEvent Completed scan for access points"));
        break;
    case SYSTEM_EVENT_STA_START:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi client started"));
        break;
    case SYSTEM_EVENT_STA_STOP:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi clients stopped"));
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        WiFiContext::setGreenPixel();
        logger.log(PSTR("WiFiContext::WiFiEvent Connected to access point"));
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        logger.log(PSTR("WiFiContext::WiFiEvent Authentication mode of access point has changed"));
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        logger.log(PSTR("WiFiContext::WiFiEvent Lost IP address and IP address is reset to 0"));
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi Protected Setup (WPS): succeeded in enrollee mode"));
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi Protected Setup (WPS): failed in enrollee mode"));
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi Protected Setup (WPS): timeout in enrollee mode"));
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi Protected Setup (WPS): pin code in enrollee mode"));
        break;
    case SYSTEM_EVENT_AP_START:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi access point started"));
        break;
    case SYSTEM_EVENT_AP_STOP:
        logger.log(PSTR("WiFiContext::WiFiEvent WiFi access point  stopped"));
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        logger.log(PSTR("WiFiContext::WiFiEvent Client connected"));
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        logger.log(PSTR("WiFiContext::WiFiEvent Client disconnected"));
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        logger.log(PSTR("WiFiContext::WiFiEvent Assigned IP address to client"));
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        logger.log(PSTR("WiFiContext::WiFiEvent Received probe request"));
        break;
    case SYSTEM_EVENT_GOT_IP6:
        logger.log(PSTR("WiFiContext::WiFiEvent IPv6 is preferred"));
        break;
    case SYSTEM_EVENT_ETH_START:
        logger.log(PSTR("WiFiContext::WiFiEvent Ethernet started"));
        break;
    case SYSTEM_EVENT_ETH_STOP:
        logger.log(PSTR("WiFiContext::WiFiEvent Ethernet stopped"));
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        logger.log(PSTR("WiFiContext::WiFiEvent Ethernet connected"));
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        logger.log(PSTR("WiFiContext::WiFiEvent Ethernet disconnected"));
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        logger.log(PSTR("WiFiContext::WiFiEvent Obtained IP address"));
        break;
    default:
        break;
    }
}

const char *WiFiContext::describeWiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        return PSTR("SYSTEM_EVENT_STA_GOT_IP");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        return PSTR("SYSTEM_EVENT_STA_DISCONNECTED");
        break;
    case SYSTEM_EVENT_WIFI_READY:
        return PSTR("SYSTEM_EVENT_WIFI_READY");
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        return PSTR("SYSTEM_EVENT_SCAN_DONE");
        break;
    case SYSTEM_EVENT_STA_START:
        return PSTR("SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_STOP:
        return PSTR("SYSTEM_EVENT_STA_STOP");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        return PSTR("SYSTEM_EVENT_STA_CONNECTED");
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        return PSTR("SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        return PSTR("SYSTEM_EVENT_STA_LOST_IP");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        return PSTR("SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        return PSTR("SYSTEM_EVENT_STA_WPS_ER_FAILED");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        return PSTR("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        return PSTR("SYSTEM_EVENT_STA_WPS_ER_PIN");
        break;
    case SYSTEM_EVENT_AP_START:
        return PSTR("SYSTEM_EVENT_AP_START");
        break;
    case SYSTEM_EVENT_AP_STOP:
        return PSTR("SYSTEM_EVENT_AP_STOP");
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        return PSTR("SYSTEM_EVENT_AP_STACONNECTED");
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        return PSTR("SYSTEM_EVENT_AP_STADISCONNECTED");
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        return PSTR("SYSTEM_EVENT_AP_STAIPASSIGNED");
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        return PSTR("SYSTEM_EVENT_AP_PROBEREQRECVED");
        break;
    case SYSTEM_EVENT_GOT_IP6:
        return PSTR("SYSTEM_EVENT_GOT_IP6");
        break;
    case SYSTEM_EVENT_ETH_START:
        return PSTR("SYSTEM_EVENT_ETH_START");
        break;
    case SYSTEM_EVENT_ETH_STOP:
        return PSTR("SYSTEM_EVENT_ETH_STOP");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        return PSTR("SYSTEM_EVENT_ETH_CONNECTED");
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        return PSTR("SYSTEM_EVENT_ETH_DISCONNECTED");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        return PSTR("SYSTEM_EVENT_ETH_GOT_IP");
        break;
    default:
        break;
    }
    return "";
}
