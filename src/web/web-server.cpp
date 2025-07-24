#include "web/web-server.h"
#include "web/web-utils.h"
#include "web/api-handlers.h"
#include "constants.h"
#include "globals.h"
#include "sensor.h"


ESP8266WebServer webServer(WEB_PORT);
WebSockets4WebServer logSocket;
WebSockets4WebServer dataSocket;

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
    /* System */
    webServer.on("/api/restart", handleRestart);
    webServer.on("/api/pause", handlePause);
    webServer.on("/api/resume", handleResume);
    webServer.on("/api/system-info", handleSystemInfo);
    webServer.on("/api/clear-old", handleClearOldData);

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