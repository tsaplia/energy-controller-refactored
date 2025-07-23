#include "wifi-tools.h"
#include "globals.h"
#include "utils.h"
#include "sensor.h"
#include "web-server.h"

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
    configs.lastSaveDay = now - now % SEC_IN_DAY + DAY_PHASE_START - TIMEZONE_OFFSET_SEC;
    configs.lastSaveNight = now - now % SEC_IN_DAY + NIGHT_PHASE_START - TIMEZONE_OFFSET_SEC;
    if(configs.lastSaveDay >= now) configs.lastSaveDay -= SEC_IN_DAY;
    if(configs.lastSaveNight >= now) configs.lastSaveNight -= SEC_IN_DAY;
    
}

/* TODO: formattedEnergy energy shoulb be without reset value */
bool writeEnergy(time_t timestamp, char phase, String formattedEnergy) {
    String payload = formatTime(timestamp, STATS_DATE_FORMAT) + ";" + String(phase) + ";" + formattedEnergy + "\n";
    return writeFile(STATS_FILENAME, payload);
}

/* save day/night stats */
void saveEnergyStats(time_t &now, SensorData &data) {
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
    // TODO: remove end

    if(saveNeeded) configs.save();
}

/* Logic for processing sensor data in the main loop */
void handleSensorData() {
    time_t now = time(nullptr);

    if(!configs.lastSaveDay || !configs.lastSaveNight) initEnergySaves(now);

    /* continue only if it's time to get data */
    if(appState.lastDataGot + DATA_SAVE_INTERVAL_SEC > now) return;

    logger.info("Getting sensor data...");
    SensorData data = getSensorData();
    if(appState.lastDataGot && data.valid){ // write data to file if not just started
        if(!writeFile(DATA_FILENAME, data.toCsv())) {
            logger.error("Failed to write data to file");
        }
    }
    appState.lastDataGot = now - now % DATA_SAVE_INTERVAL_SEC; // set even if data is not valid
    if(!data.valid) {
        logger.error("Failed to get data");
        return;
    }

    /* Socket */
    String payload = getSocketPayload(WS_TYPE_DATA, data.toJson(), false);
    dataSocket.broadcastTXT(payload);

    saveEnergyStats(now, data);
}