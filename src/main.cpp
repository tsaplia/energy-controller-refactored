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
#include "sensor.h"
#include "loop-handlers.h"

void setup() {
    Serial.begin(115200);

    // TODO: add
    // Serial.begin(9600); 
    // node.begin(0xF8, Serial);

    logger.debug("Booting Sketch...");
    
    if(LittleFS.begin()) {
        logger.debug("LittleFS mounted");
        if(configs.load()) {
            logger.logStorage.setLogLevel(configs.logLevel);
        }
        else logger.warning("No config loaded");
        
        if(!logger.logStorage.load(LOG_FILENAME)) logger.warning("Failed to load logs");
    } else {
        logger.error("Failed to mount LittleFS");
    }

    WiFi.mode(WIFI_AP_STA);
    if(!configs.ssid.isEmpty() && !configs.password.isEmpty()) {
        startWiFiSTA(configs.ssid, configs.password);
    }
    syncTime();

    setupWebServer();
    webServer.begin();
    logger.debug("Web server started");

    setupOTA();
    ArduinoOTA.begin();
    logger.debug("OTA started");

    ftpSrv.begin(FTP_USER, FTP_PASSWORD);
    logger.debug("FTP server started");

    /* Files */
    if(!LittleFS.exists(DATA_FILENAME)) {
        logger.info("Creating daily data file...");
        writeFile(DATA_FILENAME, SensorData::csvHeader());
    }
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    logSocket.loop();
    dataSocket.loop();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();

    handleWifiConnection();

    if(appState.running) { 
        // if data collection / processing is not paused
        if(!appState.timeSynced ||  millis() - appState.lastTimerSync >= TIME_SYNC_INTERVAL) syncTime(); 
        handleLogSave();    
        if(appState.timeSynced) handleSensorData();
    }
    delay(100);
}
