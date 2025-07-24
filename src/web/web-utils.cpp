#include "web/web-utils.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "web/web-server.h"
#include "globals.h"

/* check if the request is from the AP */
bool isApRequest() {
    return WiFi.softAPIP() == webServer.client().localIP();
}

/* check if the request is from the STA */
bool isStaRequest() {
    return WiFi.localIP() == webServer.client().localIP();
}

/* allow all CORS requests */
void sendCorsHeader() {
    #if ALLOW_CORS
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    #endif
}

/* redirect to the root page (for the AP) */
void sendRootRedirect() {
    logger.debug("Redirecting to /");
    webServer.sendHeader("Location", "/", true);
    webServer.send(302, "text/plain", "");
}

/* send an error to the client */
void sendMessage(int code, const char* message) {
    sendCorsHeader();
    logger.debug("Sending message " + String(code) + ": " + message);
    webServer.send(code, "application/json", "{\"detail\": \"" + String(message) + "\"}");
}

/* send the right file to the client */
void sendFile(String path){ 
    if(path.indexOf(".") == -1) path += ".html";
    logger.debug("Got file request for " + path);

    String contentType = getContentType(path);
    if(LittleFS.exists(path)){
        sendCorsHeader();
        logger.debug("Sending file " + path);
        File file = LittleFS.open(path, "r");
        webServer.streamFile(file, contentType);
        file.close();
    } else {
        sendMessage(404, "Not Found");
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

String getSocketPayload(const String& type, const String& msg, bool wrap) {
    if(wrap) return "{\"type\": \"" + type + "\", \"data\": \"" + msg + "\"}";
    return "{\"type\": \"" + type + "\", \"data\": " + msg + "}";
}

String getContentType(const String& filename) {
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