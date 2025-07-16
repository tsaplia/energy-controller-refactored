#include "web-server.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "wifi-tools.h"
#include "constants.h"
#include "globals.h"
#include <utils.h>

ESP8266WebServer webServer(WEB_PORT);
WebSockets4WebServer logSocket;

bool parseWifiCredentials(const String& body, String& ssid, String& password) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) return false;

    ssid = doc["ssid"] | "";
    password = doc["password"] | "";
    return (ssid.length() > 0 && password.length() > 0);
}

String getSocketPayload(const String& type, const String& msg) {
    return "{\"type\": \"" + type + "\", \"log\": \"" + msg + "\"}";
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

/* get current sinfo about heaf and filesystem (AP & STA) */
void handleSystemInfo() {
    JsonDocument doc;
    doc["heap_free"] = ESP.getFreeHeap();
    doc["heap_max_block"] = ESP.getMaxFreeBlockSize();
    doc["heap_fragmentation"] = ESP.getHeapFragmentation();
    FSInfo fsInfo;
    LittleFS.info(fsInfo);
    doc["fs_total_kb"] = fsInfo.totalBytes / 1024.0;
    doc["fs_used_kb"] = fsInfo.usedBytes / 1024.0;
    String json;
    serializeJson(doc, json);
    webServer.send(200, "application/json", json);
}


/* restart controller (AP & STA)*/
void handleRestart() {
    webServer.send(200, "text/json", "{\"status\": \"Restarting...\"}");
    delay(500);
    logger.warning("Restarting...");
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

    logger.logStorage.onNew([](const LogEntry& log) {
        String text = getSocketPayload(WS_TYPE_LOG, log);
        logSocket.broadcastTXT(text);
    });
}