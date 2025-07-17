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
void sendError(int code, const char* message) {
    logger.debug("Sending error " + String(code) + ": " + message);
    webServer.send(code, "application/json", "{\"error\": \"" + String(message) + "\"}");
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
        sendError(404, "Not Found");
    }
}

/* handle connect request for the AP */
void handleWifiConnect() {
    if(!isApRequest()) return sendError(403, "Forbidden");
    logger.debug("Got wifi connect request");

    if (webServer.hasArg("plain") == false) {
        return sendError(400, "Body missing");
    }

    String body = webServer.arg("plain");
    String ssid;
    String password;
    if (!parseWifiCredentials(body, ssid, password)) {
        return sendError(400, "Invalid or missing JSON fields");
    }

    if (changeWiFiSTA(ssid, password)) {
        String ipJson = "{\"ip\":\"" + WiFi.localIP().toString() + "\"}";
        webServer.send(200, "application/json", ipJson);
        delay(1000);
    } else {
        sendError(500, "Failed to connect");
    }
}

/* get current info about heaf and filesystem (AP & STA) */
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
    webServer.send(200, "text/json", "{\"status\": \"Restarting...\"}");
    logger.warning("Restarting...");
    logger.logStorage.save(LOG_FILENAME);
    delay(500);
    ESP.restart();
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

void setupWebServer() {
    webServer.on("/", handleRoot);
    webServer.on("/api/connect-wifi", HTTP_POST, handleWifiConnect);
    webServer.on("/api/restart", handleRestart);
    webServer.on("/api/system-info", handleSystemInfo);
    webServer.onNotFound(handleNotFound);

    webServer.addHook(logSocket.hookForWebserver("/ws/logs", logSocketEvent));
    webServer.addHook(dataSocket.hookForWebserver("/ws/data", dataSocketEvent));

    logger.logStorage.onNew([](const LogEntry& log) {
        String text = getSocketPayload(WS_TYPE_LOG, log);
        logSocket.broadcastTXT(text);
    });
}