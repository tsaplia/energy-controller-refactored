#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <FTPServer.h>
#include "constants.h"
#include "web-server.h"
#include "wifi-tools.h"
#include "configs.h"
#include "globals.h"

FTPServer ftpSrv(LittleFS);

void logFsInfo() {
    FSInfo fsInfo;
    LittleFS.info(fsInfo);

    logger.info("Total space: " + String(fsInfo.totalBytes / 1024.0, 2) + " KB");
    logger.info("Used space: " + String(fsInfo.usedBytes / 1024.0, 2) + " KB");
    logger.info("Free space: " + String((fsInfo.totalBytes - fsInfo.usedBytes) / 1024.0, 2) + " KB");
}

void setup() {
    Serial.begin(115200);
    logger.info("Booting Sketch...");

    if(LittleFS.begin()) {
        logger.info("LittleFS mounted");
    } else {
        logger.error("Failed to mount LittleFS");
    }

    logFsInfo();

    if(!configs.load()) {
        logger.warning("No config loaded");
    }

    WiFi.mode(WIFI_AP_STA);

    if(!configs.ssid.isEmpty() && !configs.password.isEmpty()) {
        startWiFiSTA(configs.ssid, configs.password);
    }

    setupWebServer();
    webServer.begin();
    logger.info("Web server started");

    setupOTA();
    ArduinoOTA.begin();
    logger.info("OTA started");

    ftpSrv.begin(FTP_USER, FTP_PASSWORD);
    logger.info("FTP server started");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();

    if(WiFi.status() == WL_CONNECTED && APConnected) {
        logger.warning("STA and AP are connected at the same time. Disconnecting AP.");
        disconnectAP();
    } else if (WiFi.status() != WL_CONNECTED && !APConnected) {
        logger.warning("No connection to WiFi, starting AP");
        startWiFiAP();
    }

    delay(200);
}
