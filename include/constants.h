#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ALLOW_CORS false
#define STREAM_ALL_LOGS false

#include <stdint.h>
#include <ESP8266WiFi.h>

/* Webserver */
constexpr int WEB_PORT = 80;
constexpr const char* WS_TYPE_HISTORY   = "history";
constexpr const char* WS_TYPE_LOG       = "log";
constexpr const char* WS_TYPE_DATA      = "data";

constexpr const char* AP_PASSWORD = "password";
constexpr const char* AP_SSID = "ESP8266-Sensor";
const IPAddress AP_IP(192, 168, 4, 1);

constexpr const char* OTA_HOSTNAME = "esp8266";
constexpr uint16_t OTA_PORT = 8266;

constexpr const char* FTP_USER = "esp";
constexpr const char* FTP_PASSWORD = "password";

constexpr int WIFI_CONNECT_DELAY = 500;
constexpr int WIFI_CONNECT_TRIES = 15;
constexpr byte DNS_PORT = 53;

/* Configs */
constexpr const char* CONFIGS_FILENAME = "/configs.json";

/* Data */
constexpr int DATA_SAVE_INTERVAL_SEC = 30; // 30 seconds
constexpr const char* DATA_FILENAME = "/sensor-data.csv";
constexpr int KEEP_DATA_SEC = 1*24*60*60;
constexpr int RESTART_INTERVAL_SEC = 24*60*60;

/* Day/Night stats*/
constexpr const char* STATS_FILENAME = "/energy-stats.csv";
constexpr int DAY_PHASE_START = 7*60*60; 
constexpr int NIGHT_PHASE_START = 23*60*60; 
constexpr const char* STATS_DATE_FORMAT = "%Y-%m-%d";
const char DAY_PHASE_SYMBOL = 'D';
const char NIGHT_PHASE_SYMBOL = 'N';

/* Logging */
constexpr const char* LOG_FILENAME = "/logs.txt";
constexpr const char* LOGGER_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
constexpr int MAX_LOG_ENTRIES = 200;
constexpr int LOG_SAVE_INTERVAL = 1000*60*1; // 1 minutes

/* Time */
constexpr int SEC_IN_DAY = 60*60*24;
constexpr const char* TIME_SERVER_1 = "pool.ntp.org";
constexpr const char* TIME_SERVER_2 = "time.nist.gov";
constexpr int TIMEZONE_OFFSET_SEC = 60*60*2;  // 2 hours
constexpr int TIME_SYNC_INTERVAL = 1000*60*60*5; // 5 hours
#endif