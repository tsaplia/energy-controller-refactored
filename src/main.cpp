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
#include <sensor.h>

FTPServer ftpSrv(LittleFS);
unsigned long lastLogSave = 0;
unsigned long lastDataGot = 0; // (sec) should be multiple of DATA_SAVE_INTERVAL_SEC

void setup() {
    Serial.begin(115200);

    // Serial.begin(9600); 
    // node.begin(0xF8, Serial);

    logger.debug("Booting Sketch...");
    
    if(LittleFS.begin()) {
        logger.debug("LittleFS mounted");
        if(!configs.load()) logger.warning("No config loaded");
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

    logger.info("Sketch booted");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    logSocket.loop();
    dataSocket.loop();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();

    
    if(WiFi.status() == WL_CONNECTED && APConnected) {
        logger.warning("STA and AP are connected at the same time. Disconnecting AP.");
        disconnectAP();
    } else if (WiFi.status() != WL_CONNECTED && !APConnected) {
        logger.warning("No connection to WiFi, starting AP");
        startWiFiAP();
    }
    
    if(!timeSynced ||  lastTimerSync + TIME_SYNC_INTERVAL < millis()) syncTime();

    if(lastLogSave + LOG_SAVE_INTERVAL < millis() && !logger.logStorage.isSaved()) {
        logger.info("Saving logs...");
        if(logger.logStorage.save(LOG_FILENAME)){
            lastLogSave = millis();
        } else {
            logger.warning("Failed to save logs");
        }
    }

    /* Continue only if time is synced */
    if(!timeSynced) return;

    time_t now = time(nullptr);
    if(lastDataGot + DATA_SAVE_INTERVAL_SEC <= now){
        // TODO: save data but only if lastdagot != 0
        logger.info("Getting sensor data...");
        SensorData data = getSensorData();

        lastDataGot = now - now % DATA_SAVE_INTERVAL_SEC; // set even if data is not valid

        if(data.valid) {
            String payload = getSocketPayload(WS_TYPE_DATA, data.toJson(), false);
            dataSocket.broadcastTXT(payload);
        } else {
            logger.error("Failed to get data");
        }
    }
}
