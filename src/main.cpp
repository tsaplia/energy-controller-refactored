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

    // TODO: add
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

    /* Files */
    if(!LittleFS.exists(DATA_FILENAME)) {
        logger.info("Creating daily data file...");
        writeFile(DATA_FILENAME, SensorData::csvHeader());
    }

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

    if(!running) return; // controller is paused
    
    if(!timeSynced ||  lastTimerSync + TIME_SYNC_INTERVAL < millis()) syncTime();

    /* Save logs */ //TODO: move
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

    /* if we just started counting */ // TODO: move
    if(!configs.lastSaveDay || !configs.lastSaveNight) { 
        logger.info("Initialiying last energy saves...");
        configs.lastSaveDay = now - now % SEC_IN_DAY + DAY_PHASE_START - TIMEZONE_OFFSET_SEC;
        configs.lastSaveNight = now - now % SEC_IN_DAY + NIGHT_PHASE_START - TIMEZONE_OFFSET_SEC;
        if(configs.lastSaveDay >= now) configs.lastSaveDay -= SEC_IN_DAY;
        if(configs.lastSaveNight >= now) configs.lastSaveNight -= SEC_IN_DAY;
    }

    /* continue only if it's time to get data */
    if(lastDataGot + DATA_SAVE_INTERVAL_SEC > now) return;

    logger.info("Getting sensor data...");
    SensorData data = getSensorData();
    if(lastDataGot && data.valid){ // write data to file if not just started
        writeFile(DATA_FILENAME, data.toCsv());
    }
    lastDataGot = now - now % DATA_SAVE_INTERVAL_SEC; // set even if data is not valid
    if(!data.valid) {
        logger.error("Failed to get data");
        return;
    }

    /* Socket */
    String payload = getSocketPayload(WS_TYPE_DATA, data.toJson(), false);
    dataSocket.broadcastTXT(payload);

    /* Saving day/night data */ // TODO: move to another function
    bool saveNeeded = false;
    while(configs.lastSaveDay + SEC_IN_DAY <= now){
        saveNeeded = true;
        logger.info("Saving day data for " + formatTime(configs.lastSaveDay, STATS_DATE_FORMAT) + "...");
        configs.lastSaveDay += SEC_IN_DAY;
        writeEnergy(configs.lastSaveDay, DAY_PHASE_SYMBOL, data.getFormattedEnergy());
    } 
    while(configs.lastSaveNight + SEC_IN_DAY <= now){
        saveNeeded = true;
        logger.info("Saving night data for " + formatTime(configs.lastSaveNight, STATS_DATE_FORMAT) + "...");
        configs.lastSaveNight += SEC_IN_DAY;
        writeEnergy(configs.lastSaveNight, NIGHT_PHASE_SYMBOL, data.getFormattedEnergy());
    }

    // TODO: remove
    Serial.println(String(configs.resetEnergyValue)+ ", " +data.energy + String(", ") +data.getFormattedEnergy());
    configs.resetEnergyValue -= 20;
    saveNeeded = true;

    if(saveNeeded) configs.save();
}
