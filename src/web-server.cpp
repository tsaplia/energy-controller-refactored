#include "web-server.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "wifi-tools.h"
#include "constants.h"
#include "globals.h"
#include <utils.h>
#include <sensor.h>

ESP8266WebServer webServer(WEB_PORT);
WebSockets4WebServer logSocket;
WebSockets4WebServer dataSocket;

bool parseWifiCredentials(const String& body, String& ssid, String& password) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) return false;

    ssid = doc["ssid"] | "";
    password = doc["password"] | "";
    return (ssid.length() > 0 && password.length() > 0);
}

String getSocketPayload(const String& type, const String& msg, bool wrap) {
    if(wrap) return "{\"type\": \"" + type + "\", \"data\": \"" + msg + "\"}";
    return "{\"type\": \"" + type + "\", \"data\": " + msg + "}";
}

/* event handler for the socket on /log */
void logSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    IPAddress ip = logSocket.remoteIP(num);
    if(type == WStype_CONNECTED) {
        String text = getSocketPayload(WS_TYPE_HISTORY, logger.logStorage.getLogs());
        logSocket.sendTXT(num, text);
        logger.debug("Connected to log socket: " + ip.toString());
    } else if (type == WStype_DISCONNECTED) {
        logger.debug("Someone disconnected from log socket");
    }
}

/* event handler for the socket on /data */
void dataSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    IPAddress ip = dataSocket.remoteIP(num);
    if(type == WStype_CONNECTED) {
        String text = getSocketPayload(WS_TYPE_DATA, getSensorData().toJson(), false);
        dataSocket.sendTXT(num, text);
        logger.debug("Connected to data socket: " + ip.toString());
    } else if (type == WStype_DISCONNECTED) {
        logger.debug("Someone disconnected from data socket");
    }
}

/* check if the request is from the AP */
bool isApRequest() {
    return WiFi.softAPIP() == webServer.client().localIP();
}

/* check if the request is from the STA */
bool isStaRequest() {
    return WiFi.localIP() == webServer.client().localIP();
}

/* redirect to the root page (for the AP) */
void sendRootRedirect() {
    logger.debug("Redirecting to /");
    webServer.sendHeader("Location", "/", true);
    webServer.send(302, "text/plain", "");
}

/* send an error to the client */
void sendMessage(int code, const char* message) {
    logger.debug("Sending message " + String(code) + ": " + message);
    webServer.send(code, "application/json", "{\"detail\": \"" + String(message) + "\"}");
}

/* send the right file to the client */
void sendFile(String path){ 
    if(path.indexOf(".") == -1) path += ".html";
    logger.debug("Got file request for " + path);

    String contentType = getContentType(path);
    if(LittleFS.exists(path)){
        logger.debug("Sending file " + path);
        File file = LittleFS.open(path, "r");
        webServer.streamFile(file, contentType);
        file.close();
    } else {
        sendMessage(404, "Not Found");
    }
}

void sendCorsHeader() {
    #if ALLOW_CORS
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    #endif
}

/* handle connect request for the (AP&STA) */
void handleWifiConnect() {
    sendCorsHeader();
    logger.debug("Got wifi connect request");

    if (webServer.hasArg("plain") == false) {
        return sendMessage(400, "Body missing");
    }

    String body = webServer.arg("plain");
    String ssid;
    String password;
    if (!parseWifiCredentials(body, ssid, password)) {
        return sendMessage(400, "Invalid or missing JSON fields");
    }

    if (changeWiFiSTA(ssid, password)) {
        String ipJson = "{\"ip\":\"" + WiFi.localIP().toString() + "\"}";
        webServer.send(200, "application/json", ipJson);
        delay(1000);
    } else {
        sendMessage(500, "Failed to connect");
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

void handleSettingsSet() {
    sendCorsHeader();

    if(!isStaRequest()) return sendMessage(403, "Forbidden");
    if(!webServer.hasArg("plain")) return sendMessage(400, "Body missing");
    logger.debug("Got settings set request");

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, webServer.arg("plain"));
    if(error) return sendMessage(400, "JSON parse error");

    if(doc["logLevel"].is<int>()) {
        if(!logger.logStorage.setLogLevel(doc["logLevel"])) {
            return sendMessage(400, "Invalid log level");
        }
        configs.logLevel = doc["logLevel"];
    };
    if(doc["dataSaveInterval"].is<int>()) {
        configs.dataSaveInterval = doc["dataSaveInterval"];
    };
    if(doc["keepData"].is<int>()) {
        configs.keepData = doc["keepData"];
    };

    int _dayPhaseOffset = configs.dayPhaseStart - configs.timezoneOffset;
    int _nightPhaseOffset = configs.nightPhaseStart - configs.timezoneOffset;
    if(doc["timezoneOffset"].is<int>()) {
        configs.timezoneOffset = doc["timezoneOffset"];
        syncTime();
    };
    if(doc["dayPhaseStart"].is<int>()) {
        configs.dayPhaseStart = doc["dayPhaseStart"];
    };
    if(doc["nightPhaseStart"].is<int>()) {
        configs.nightPhaseStart = doc["nightPhaseStart"];
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
    logger.warning("Restarting...");
    logger.logStorage.save(LOG_FILENAME);
    delay(1000);
    ESP.restart();
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

/* root handler for AP & STA */
void handleRoot() {
    if(isApRequest()) {
        sendFile("/auth.html");
    } else {
        sendFile("/index.html");
    }
}


/* handle not found requests for the AP & STA */
void handleNotFound() {
    if(isApRequest()) {
        sendRootRedirect();
    } else {
        sendFile(webServer.uri());
    }
}

void optionsHandler() {
    #if ALLOW_CORS
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, OPTIONS");
    webServer.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    #endif
    webServer.send(204);
}

void setupWebServer() {
    /* System */
    webServer.on("/api/restart", handleRestart);
    webServer.on("/api/pause", handlePause);
    webServer.on("/api/resume", handleResume);
    webServer.on("/api/system-info", handleSystemInfo);

    /* Pages + API */
    webServer.on("/", handleRoot);
    webServer.on("/api/connect-wifi", HTTP_POST, handleWifiConnect);
    webServer.on("/api/connect-wifi", HTTP_OPTIONS, optionsHandler);
    webServer.on("/api/settings", HTTP_GET, handleSettingsGet);
    webServer.on("/api/settings", HTTP_POST, handleSettingsSet);
    webServer.on("/api/settings", HTTP_OPTIONS, optionsHandler);
    webServer.on("/api/reset-energy", HTTP_PUT, handleResetEnergy);
    webServer.on("/api/reset-energy", HTTP_OPTIONS, optionsHandler);
    webServer.onNotFound(handleNotFound);

    /* Sockets */
    webServer.addHook(logSocket.hookForWebserver("/ws/logs", logSocketEvent));
    webServer.addHook(dataSocket.hookForWebserver("/ws/data", dataSocketEvent));

    logger.logStorage.onNew([](const LogEntry& log) {
        String text = getSocketPayload(WS_TYPE_LOG, log);
        logSocket.broadcastTXT(text);
    });
}