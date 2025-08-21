#include "web/api-handlers.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "web/web-server.h"
#include "web/web-utils.h"
#include "globals.h"
#include "wifi-tools.h"
#include "sensor.h"
#include "utils.h"

/* handle connect request for the (AP&STA) */
void handleWifiConnect() {
    sendCorsHeader();
    if (!webServer.hasArg("plain")) return sendMessage(400, "Body missing");
    logger.debug("Got wifi connect request");

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, webServer.arg("plain"));
    if(error) return sendMessage(400, "JSON parse error");
    String ssid = doc["ssid"] | "";
    String password = doc["password"] | "";
    if(ssid.length() == 0 || password.length() == 0) return sendMessage(400, "Missing credentials");

    if (isStaRequest()) sendMessage(200, "OK"); // won't be able to see result in STA mode

    if (changeWiFiSTA(ssid, password)) {
        String ipJson = "{\"ip\":\"" + WiFi.localIP().toString() + "\"}";
        if(isApRequest()) webServer.send(200, "application/json", ipJson);
        delay(500);
    } else {
        if(isApRequest()) sendMessage(500, "Failed to connect");
    }
}

/* get saved settings (STA) */
void handleSettingsGet() {
    sendCorsHeader();
    if(!isStaRequest()) return sendMessage(403, "Forbidden");
    logger.debug("Got settings get request");

    JsonDocument doc;
    configs.toJson(doc, true);
    String output;
    serializeJson(doc, output);
    webServer.send(200, "application/json", output);
}

/* set saved settings (STA) */
void handleSettingsSet() {
    sendCorsHeader();

    if(!isStaRequest()) return sendMessage(403, "Forbidden");
    if(!webServer.hasArg("plain")) return sendMessage(400, "Body missing");
    logger.debug("Got settings set request");

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, webServer.arg("plain"));
    if(error) return sendMessage(400, "JSON parse error");

    int _dayPhaseOffset = configs.dayPhaseStart - configs.timezoneOffset;
    int _nightPhaseOffset = configs.nightPhaseStart - configs.timezoneOffset;

    if(doc["logLevel"].is<int>() && !logger.logStorage.setLogLevel(doc["logLevel"])) {
        return sendMessage(400, "Invalid log level");
    };

    configs.fromJson(doc, true);
    if(doc["timezoneOffset"].is<int>()) {
        syncTime();
    };

    configs.lastSaveDay += configs.dayPhaseStart - configs.timezoneOffset - _dayPhaseOffset;
    configs.lastSaveNight += configs.nightPhaseStart - configs.timezoneOffset - _nightPhaseOffset;

    configs.save();
    sendMessage(200, "OK");
}

/* reset energy (STA) */
void handleResetEnergy() {
    if(!isStaRequest()) return sendMessage(403, "Forbidden");
    logger.warning("Resetting energy...");
    configs.resetEnergyValue = getSensorData().energy;
    configs.save();
    return sendMessage(200, "OK");
}

/* get current info about heap and filesystem (AP & STA) */
void handleSystemInfo() {
    JsonDocument doc;

    doc["chip"]["id"] = ESP.getChipId();
    doc["chip"]["core_version"] = ESP.getCoreVersion();
    doc["chip"]["sdk_version"] = ESP.getSdkVersion();
    doc["chip"]["cpu_freq_mhz"] = ESP.getCpuFreqMHz();

    doc["flash"]["chip_size_kb"] = ESP.getFlashChipSize() / 1024;
    doc["flash"]["chip_speed_mhz"] = ESP.getFlashChipSpeed() / 1000000;
    doc["flash"]["sketch_size_kb"] = ESP.getSketchSize() / 1024;
    doc["flash"]["free_sketch_space_kb"] = ESP.getFreeSketchSpace() / 1024;
 
    doc["ram"]["heap_free_kb"] = ESP.getFreeHeap() / 1024.0;
    doc["ram"]["heap_max_block_kb"] = ESP.getMaxFreeBlockSize() / 1024.0;
    doc["ram"]["heap_fragmentation_percent"] = ESP.getHeapFragmentation();

    FSInfo fsInfo;
    LittleFS.info(fsInfo);
    doc["fs"]["total_kb"] = fsInfo.totalBytes / 1024.0;
    doc["fs"]["used_kb"] = fsInfo.usedBytes / 1024.0;

    String json;
    serializeJson(doc, json);
    webServer.send(200, "application/json", json);
}

/* restart controller (AP & STA)*/
void handleRestart() {
    sendMessage(200, "Restarting...");
    restart();
}

/* clear old sensor data (AP & STA)*/
void handleClearOldData() {
    if(clearOldData()) sendMessage(200, "Cleared old data");
    else sendMessage(500, "Failed to clear old data");
}

/* pause controller (AP & STA)*/
void handlePause() {
    if(!appState.running) return sendMessage(403, "Already paused");
    appState.running = false;
    sendMessage(200, "Pausing...");
    logger.warning("Pausing...");
}

/* resume controller (AP & STA)*/
void handleResume() {
    if(appState.running) return sendMessage(403, "Already running");
    appState.running = true;
    sendMessage(200, "Resuming...");
    logger.warning("Resuming...");
}