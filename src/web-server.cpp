#include "web-server.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "wifi-tools.h"
#include "constants.h"
#include "globals.h"

ESP8266WebServer webServer(WEB_PORT);

String getContentType(String filename){
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".csv")) return "text/csv";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

bool parseWifiCredentials(const String& body, String& ssid, String& password) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) return false;

    ssid = doc["ssid"] | "";
    password = doc["password"] | "";
    return (ssid.length() > 0 && password.length() > 0);
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
    logger.warning("Sending error " + String(code) + ": " + message);
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

/* root handler for the AP & STA */
void handleRoot() {
    if(isApRequest()) {
        sendFile("/auth.html");
    } else {
        sendFile("/index.html");
    }
}

void handleLogs() {
    webServer.send(200, "text/plain", logger.getLogs());
}

void handleRestart() {
    webServer.send(200, "text/json", "{\"status\": \"Restarting...\"}");
    delay(500);
    logger.warning("Restarting...");
    ESP.restart();
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
    webServer.on("/all-logs", handleLogs);
    webServer.onNotFound(handleNotFound);
}