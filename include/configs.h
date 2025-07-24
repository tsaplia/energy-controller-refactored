#ifndef CONFIGS_H
#define CONFIGS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "logger.h"
#include "constants.h"

class Configs {
public:
    Configs(const char* filename);

    bool load();
    bool save() const;
    void toJson(JsonDocument& doc, bool settingsOnly = false) const;
    void fromJson(const JsonDocument& doc, bool settingsOnly = false);

    /* Wifi */
    String ssid;
    String password;
    /* Sensor*/
    uint32_t resetEnergyValue;  // energy value when last reset
    time_t lastSaveDay;         // time when last day data saved (07:00)
    time_t lastSaveNight;       // time when last night data saved (23:00)
    /* Settings */
    int logLevel = LOG_LEVEL_INFO;
    int dataSaveInterval = DATA_SAVE_INTERVAL_SEC;
    int timezoneOffset = TIMEZONE_OFFSET_SEC;
    int dayPhaseStart = DAY_PHASE_START;
    int nightPhaseStart = NIGHT_PHASE_START;
    int keepData = KEEP_DATA_SEC;
private:
    const char* filename;
};

#endif
