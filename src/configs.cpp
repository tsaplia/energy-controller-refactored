#include "configs.h"
#include <LittleFS.h>
#include "globals.h"

Configs::Configs(const char* filename): filename(filename) {}

bool Configs::load() {
    File file = LittleFS.open(filename, "r");
    if (!file) {
        logger.error("Failed to open config file for reading");
        return false;
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        logger.error("Failed to parse JSON: " + String(error.c_str()));
        return false;
    }

    ssid = doc["ssid"] | "";
    password = doc["password"] | "";

    lastSaveDay = doc["lastSaveDay"] | 0;
    lastSaveNight = doc["lastSaveNight"] | 0;
    resetEnergyValue = static_cast<uint32_t>(doc["resetEnergyValue"]) | 0;

    logLevel = doc["logLevel"] | LOG_LEVEL_INFO;
    dataSaveInterval = doc["dataSaveInterval"] | DATA_SAVE_INTERVAL_SEC;
    keepData = doc["keepData"] | KEEP_DATA_SEC;
    timezoneOffset = doc["timezoneOffset"] | TIMEZONE_OFFSET_SEC;
    dayPhaseStart = doc["dayPhaseStart"] | DAY_PHASE_START;
    nightPhaseStart = doc["nightPhaseStart"] | NIGHT_PHASE_START;
    return true;
}

bool Configs::save() const {
   File file = LittleFS.open(filename, "w");
    if (!file) {
        logger.error("Failed to open config file for writing");
        return false;
    }

    JsonDocument doc;
    toJson(doc);

    if (serializeJson(doc, file) == 0) {
        logger.error("Failed to write JSON to file");
        file.close();
        return false;
    }

    file.close();
    logger.info("Config saved");
    return true;
}

void Configs::toJson(JsonDocument& doc, bool settingsOnly) const {
    doc["logLevel"] = logLevel;
    doc["dataSaveInterval"] = dataSaveInterval;
    doc["keepData"] = keepData;
    doc["timezoneOffset"] = timezoneOffset;
    doc["dayPhaseStart"] = dayPhaseStart;
    doc["nightPhaseStart"] = nightPhaseStart;

    if(!settingsOnly) {
        doc["ssid"] = ssid;
        doc["password"] = password;
        doc["lastSaveDay"] = lastSaveDay;
        doc["lastSaveNight"] = lastSaveNight;
        doc["resetEnergyValue"] = resetEnergyValue;
    }
}
