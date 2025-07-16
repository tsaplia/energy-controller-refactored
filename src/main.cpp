#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <FTPServer.h>
#include "constants.h"
#include "web-server.h"
#include "wifi-tools.h"
#include "configs.h"
#include "globals.h"
#include "utils.h"

FTPServer ftpSrv(LittleFS);
unsigned long lastLogSave = 0;
unsigned long lastTimerSync = 0;

void setup() {
    Serial.begin(115200);

    logger.debug("Booting Sketch...");
    
    if(LittleFS.begin()) {
        logger.debug("LittleFS mounted");
        logSystemInfo(); // TODO: Move to web server
        if(!configs.load()) logger.warning("No config loaded");
        if(!logger.logStorage.load(LOG_FILENAME)) logger.warning("Failed to load logs");
    } else {
        logger.error("Failed to mount LittleFS");
    }

    WiFi.mode(WIFI_AP_STA);
    if(!configs.ssid.isEmpty() && !configs.password.isEmpty()) {
        startWiFiSTA(configs.ssid, configs.password);
    }
    if(WiFi.status() == WL_CONNECTED) {
        syncTime();
        lastTimerSync = millis();
    }

    setupWebServer();
    webServer.begin();
    logger.debug("Web server started");

    setupOTA();
    ArduinoOTA.begin();
    logger.debug("OTA started");

    ftpSrv.begin(FTP_USER, FTP_PASSWORD);
    logger.debug("FTP server started");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    logSocket.loop();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();

    if(WiFi.status() == WL_CONNECTED && APConnected) {
        logger.warning("STA and AP are connected at the same time. Disconnecting AP.");
        disconnectAP();
    } else if (WiFi.status() != WL_CONNECTED && !APConnected) {
        logger.warning("No connection to WiFi, starting AP");
        startWiFiAP();
    }

    if(lastTimerSync + TIME_SYNC_INTERVAL < millis() || lastTimerSync == 0) {
        if(WiFi.status() == WL_CONNECTED) {
            syncTime();
            lastTimerSync = millis();
        }
    }
    if(lastLogSave + LOG_SAVE_INTERVAL < millis() && !logger.logStorage.isSaved()) {
        logger.info("Saving logs...");
        if(logger.logStorage.save(LOG_FILENAME)){
            lastLogSave = millis();
        } else {
            logger.warning("Failed to save logs");
        }
    }
}
