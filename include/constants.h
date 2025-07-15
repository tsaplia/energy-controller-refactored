#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <ESP8266WiFi.h>

constexpr int WEB_PORT = 80;
constexpr const char* WS_TYPE_HISTORY   = "history";
constexpr const char* WS_TYPE_LOG       = "log";
constexpr const char* WS_TYPE_DATA      = "data";

constexpr const char* AP_PASSWORD = "password";
constexpr const char* AP_SSID = "ESP8266-Sensor";
const IPAddress AP_IP(192, 168, 4, 1);

constexpr const char* OTA_HOSTNAME = "esp8266";
constexpr uint16_t OTA_PORT = 8266;

constexpr const char* FTP_USER = "esp8266";
constexpr const char* FTP_PASSWORD = "password";

constexpr int WIFI_CONNECT_DELAY = 500;
constexpr int WIFI_CONNECT_TRIES = 15;

constexpr byte DNS_PORT = 53;

constexpr const char* CONFIGS_FILENAME = "/configs.json";

constexpr int MAX_LOG_ENTRIES = 100;
constexpr int LOG_SAVE_INTERVAL = 1000*60*5;    // 5 minutes

constexpr const char* TIME_SERVER_1 = "pool.ntp.org";
constexpr const char* TIME_SERVER_2 = "time.nist.gov";
constexpr const int TIMEZONE_OFFSET_SEC = 60*60*2;  // 2 hours
constexpr int TIME_SYNC_INTERVAL = 1000*60*60*6;    // 6 hours

#endif