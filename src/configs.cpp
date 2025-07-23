#include "configs.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
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
    return true;
}

bool Configs::save() const {
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["lastSaveDay"] = lastSaveDay;
    doc["lastSaveNight"] = lastSaveNight;
    doc["resetEnergyValue"] = resetEnergyValue;

    File file = LittleFS.open(filename, "w");
    if (!file) {
        logger.error("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        logger.error("Failed to write JSON to file");
        file.close();
        return false;
    }

    file.close();
    logger.info("Config saved");
    return true;
}
