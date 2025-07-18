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

constexpr const char* FTP_USER = "";
constexpr const char* FTP_PASSWORD = "";

constexpr int WIFI_CONNECT_DELAY = 500;
constexpr int WIFI_CONNECT_TRIES = 15;
constexpr byte DNS_PORT = 53;

constexpr const char* CONFIGS_FILENAME = "/configs.json";
constexpr int DATA_SAVE_INTERVAL_SEC = 30; // 30 seconds

constexpr const char* LOG_FILENAME = "/logs.txt";
constexpr const char* LOGGER_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
constexpr int MAX_LOG_ENTRIES = 100;
constexpr int LOG_SAVE_INTERVAL = 1000*60*1; // 1 minutes
constexpr bool STREAM_ALL_LOGS = true; // needs to be changed

constexpr const char* TIME_SERVER_1 = "pool.ntp.org";
constexpr const char* TIME_SERVER_2 = "time.nist.gov";
constexpr int TIMEZONE_OFFSET_SEC = 60*60*2;  // 2 hours
constexpr int TIME_SYNC_INTERVAL = 1000*60*60*5; // 5 hours

#endif