#include "wifi-tools.h"
#include "globals.h"
#include "utils.h"
#include "sensor.h"
#include "web/web-server.h"
#include "web/web-utils.h"

/* Logic for handling WiFi connection in the main loop */
void handleWifiConnection() {
    if(WiFi.status() == WL_CONNECTED && APConnected) {
        logger.warning("STA and AP are connected at the same time. Disconnecting AP.");
        disconnectAP();
    } else if (WiFi.status() != WL_CONNECTED && !APConnected) {
        logger.warning("No connection to WiFi, starting AP");
        startWiFiAP();
    }
}

/* Logic for log saving in the main loop */
void handleLogSave() {
    if(millis() - appState.lastLogSave >= LOG_SAVE_INTERVAL  && !logger.logStorage.isSaved()) {
        logger.info("Saving logs...");
        if(logger.logStorage.save(LOG_FILENAME)){
            appState.lastLogSave = millis();
        } else {
            logger.warning("Failed to save logs");
        }
    }
}

/* if we just started counting */ 
void initEnergySaves(time_t &now) {
    logger.info("Initialiying last energy saves...");
    configs.lastSaveDay = now - now % SEC_IN_DAY + configs.dayPhaseStart - configs.timezoneOffset;
    configs.lastSaveNight = now - now % SEC_IN_DAY + configs.nightPhaseStart - configs.timezoneOffset;
    if(configs.lastSaveDay >= now) configs.lastSaveDay -= SEC_IN_DAY;
    if(configs.lastSaveNight >= now) configs.lastSaveNight -= SEC_IN_DAY;
    
}

/* TODO: formattedEnergy energy shoulb be without reset value */
bool writeEnergy(time_t timestamp, char phase, const String energy) {
    String payload = formatTime(timestamp, STATS_DATE_FORMAT) + ";" + String(phase) + ";" + energy + "\n";
    return writeFile(STATS_FILENAME, payload);
}

/* save day/night stats */
void saveEnergyStats(time_t &now, SensorData &data) {
    bool saveNeeded = false;
    while(configs.lastSaveDay + SEC_IN_DAY <= now){
        saveNeeded = true;
        configs.lastSaveDay += SEC_IN_DAY;
        logger.info("Saving day data for " + formatTime(configs.lastSaveDay, STATS_DATE_FORMAT) + "...");
        writeEnergy(configs.lastSaveDay, DAY_PHASE_SYMBOL, data.getTotalEnergy());
    } 
    while(configs.lastSaveNight + SEC_IN_DAY <= now){
        saveNeeded = true;
        configs.lastSaveNight += SEC_IN_DAY;
        logger.info("Saving night data for " + formatTime(configs.lastSaveNight, STATS_DATE_FORMAT) + "...");
        writeEnergy(configs.lastSaveNight, NIGHT_PHASE_SYMBOL, data.getTotalEnergy());
    }

    if(saveNeeded) configs.save();
}

/* Logic for processing sensor data in the main loop */
void handleSensorData() {
    time_t now = time(nullptr);

    if(!configs.lastSaveDay || !configs.lastSaveNight) initEnergySaves(now);

    /* continue only if it's time to get data */
    if(appState.lastDataGot + configs.dataSaveInterval > now) return;

    logger.info("Getting sensor data...");
    SensorData data = getSensorData();
    if(appState.lastDataGot && data.valid){ // write data to file if not just started
        if(!writeFile(DATA_FILENAME, data.toCsv())) {
            logger.error("Failed to write data to file");
        }
    }
    appState.lastDataGot = now - now % configs.dataSaveInterval; // set even if data is not valid
    if(!data.valid) {
        logger.error("Failed to get data");
        return;
    }

    /* Socket */
    String payload = getSocketPayload(WS_TYPE_DATA, data.toJson(), false);
    dataSocket.broadcastTXT(payload);

    saveEnergyStats(now, data);
}