#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ALLOW_CORS false        // Allow CORS requests (false = disabled)
#define STREAM_ALL_LOGS false   // Stream all logs in real-time (false = disabled)

#include <stdint.h>
#include <ESP8266WiFi.h>

/* Webserver */
constexpr int WEB_PORT = 80;                        // Web server port
constexpr const char* WS_TYPE_HISTORY   = "history";
constexpr const char* WS_TYPE_LOG       = "log";  
constexpr const char* WS_TYPE_DATA      = "data"; 

constexpr const char* AP_PASSWORD = "password";     // WiFi Access Point password
constexpr const char* AP_SSID = "ESP8266-Sensor";   // WiFi Access Point name
const IPAddress AP_IP(192, 168, 4, 1);              // Access Point IP address

constexpr const char* OTA_HOSTNAME = "esp8266";     // OTA update hostname
constexpr uint16_t OTA_PORT = 8266;                 // OTA update port

constexpr const char* FTP_USER = "esp";             // FTP server username
constexpr const char* FTP_PASSWORD = "password";    // FTP server password

constexpr int WIFI_CONNECT_DELAY = 500;             // Delay between WiFi retries (ms)
constexpr int WIFI_CONNECT_TRIES = 15;              // Max WiFi connection attempts
constexpr byte DNS_PORT = 53;                       // DNS server port

/* Configs */
constexpr const char* CONFIGS_FILENAME = "/configs.json"; // Config file path

/* Data */
constexpr int DATA_SAVE_INTERVAL_SEC = 30;          // Interval to save data (sec)
constexpr const char* DATA_FILENAME = "/sensor-data.csv"; // Sensor data file
constexpr int KEEP_DATA_SEC = 1*24*60*60;           // Default data retention (sec)
constexpr int RESTART_INTERVAL_SEC = 24*60*60;      // Restart every 24 hours (sec)

/* Day/Night stats */
constexpr const char* STATS_FILENAME = "/energy-stats.csv"; // Stats file
constexpr int DAY_PHASE_START = 7*60*60;             // Default day phase start (sec)
constexpr int NIGHT_PHASE_START = 23*60*60;          // Default night phase start (sec)
constexpr const char* STATS_DATE_FORMAT = "%Y-%m-%d";
const char DAY_PHASE_SYMBOL = 'D';
const char NIGHT_PHASE_SYMBOL = 'N';

/* Logging */
constexpr const char* LOG_FILENAME = "/logs.txt";            // File path for logs
constexpr const char* LOGGER_TIME_FORMAT = "%Y-%m-%d %H:%M:%S"; // Format for log timestamps
constexpr int MAX_LOG_ENTRIES = 200;                         // Maximum number of log entries stored
constexpr int LOG_SAVE_INTERVAL = 1000*60*1;                 // Interval to save logs in ms (1 minute)

/* Time */
constexpr int SEC_IN_DAY = 60*60*24;                         // Number of seconds in one day
constexpr const char* TIME_SERVER_1 = "pool.ntp.org";        // First NTP time server
constexpr const char* TIME_SERVER_2 = "time.nist.gov";       // Second NTP time server
constexpr int TIMEZONE_OFFSET_SEC = 60*60*2;                 // Default timezone offset in seconds (2 hours)
constexpr int TIME_SYNC_INTERVAL = 1000*60*60*5;             // Interval to sync time in ms (5 hours)

#endif