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

    fromJson(doc);
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
    Serial.println(doc.as<String>());
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

template<typename T>
bool assignIfExists(const JsonDocument& doc, const char* key, T& target) {
    if(doc[key].is<T>()) {
        target = doc[key].as<T>();
        return true;
    }
    return false;
}

void Configs::fromJson(const JsonDocument &doc, bool settingsOnly) {
    assignIfExists(doc, "logLevel", logLevel);
    assignIfExists(doc, "dataSaveInterval", dataSaveInterval);
    assignIfExists(doc, "keepData", keepData);
    assignIfExists(doc, "timezoneOffset", timezoneOffset);
    assignIfExists(doc, "dayPhaseStart", dayPhaseStart);
    assignIfExists(doc, "nightPhaseStart", nightPhaseStart);
    if(!settingsOnly) {
        assignIfExists(doc, "ssid", ssid);
        assignIfExists(doc, "password", password);
        assignIfExists(doc, "lastSaveDay", lastSaveDay);
        assignIfExists(doc, "lastSaveNight", lastSaveNight);
        assignIfExists(doc, "resetEnergyValue", resetEnergyValue);
    }
}
