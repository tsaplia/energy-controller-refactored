#include "configs.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

Configs::Configs(const char* filename): filename(filename) {}

bool Configs::load() {
    if (!LittleFS.exists(filename)) {
        Serial.println("Config file does not exist");
        return false;
    }
    File file = LittleFS.open(filename, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return false;
    }

    ssid = doc["ssid"] | "";
    password = doc["password"] | "";
    Serial.println(String("Loaded config: ") + doc.as<String>());
    return true;
}

bool Configs::save() const {
    Serial.print("Saving: ");
    Serial.print(ssid);
    Serial.print(" ");
    Serial.println(password);
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;

    File file = LittleFS.open(filename, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write JSON to file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Config updated");
    return true;
}
