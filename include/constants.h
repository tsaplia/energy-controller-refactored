#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

constexpr uint16_t OTA_PORT = 8266;

constexpr int WEB_PORT = 80;

constexpr const char* AP_PASSWORD = "password";
constexpr const char* AP_SSID = "ESP8266-Sensor";
const IPAddress AP_IP(192, 168, 4, 1);

constexpr int WIFI_CONNECT_DELAY = 500;
constexpr int WIFI_CONNECT_TRIES = 10;

constexpr byte DNS_PORT = 53;


#endif