
#include "utils.h"
#include <LittleFS.h>
#include <Esp.h>
#include "globals.h"
#include <wifi-tools.h>
#include "sensor.h"

void logSystemInfo() {
    logger.debug("== Chip Info ==");
    logger.debug("Chip ID: " + String(ESP.getChipId()));
    logger.debug("Core version: " + String(ESP.getCoreVersion()));
    logger.debug("SDK version: " + String(ESP.getSdkVersion()));
    logger.debug("CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz");

    logger.debug("== Flash Info ==");
    logger.debug("Flash chip size: " + String(ESP.getFlashChipSize() / 1024) + " KB");
    logger.debug("Flash chip speed: " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz");
    logger.debug("Sketch size: " + String(ESP.getSketchSize() / 1024) + " KB");
    logger.debug("Free sketch space: " + String(ESP.getFreeSketchSpace() / 1024) + " KB");

    logger.debug("== RAM Info ==");
    logger.debug("Free heap: " + String(ESP.getFreeHeap() / 1024.0, 2) + " KB");
    logger.debug("Max free block size: " + String(ESP.getMaxFreeBlockSize() / 1024.0, 2) + " KB");
    logger.debug("Heap fragmentation: " + String(ESP.getHeapFragmentation()) + " %");

    logger.debug("== Filesystem Info ==");
    FSInfo fsInfo;
    LittleFS.info(fsInfo);
    float totalKB = fsInfo.totalBytes / 1024.0;
    float usedKB = fsInfo.usedBytes / 1024.0;
    logger.debug("Total space: " + String(totalKB, 2) + " KB");
    logger.debug("Used space: " + String(usedKB, 2) + " KB");
}
  
String formatTime(time_t t, const char* format) {
    char buf[25];
    struct tm* tmInfo = localtime(&t);
    strftime(buf, sizeof(buf), format, tmInfo);
    return String(buf);
}

bool syncTime() {
    if(WiFi.status() != WL_CONNECTED) return false;

    configTime(configs.timezoneOffset, 0, TIME_SERVER_1, TIME_SERVER_2);
    while (time(nullptr) < 100000) delay(100);

    if(!appState.timeSynced) {
        appState.lastRestart = time(nullptr) - time(nullptr) % RESTART_INTERVAL_SEC;
    }

    appState.lastTimerSync = millis();
    appState.timeSynced = true;
    logger.info("Time updated");
    return true;
}

void restart() {
    logger.warning("Restarting...");
    logger.logStorage.save(LOG_FILENAME);
    delay(1000);
    ESP.restart();
}

bool writeFile(const char* path, String message) {
    File file = LittleFS.open(path, "a");
    if (!file) {
        logger.error("Failed to open file for writing: " + String(path));
        return false;
    }
    file.print(message);
    file.close();
    return true;
}

bool clearOldData(){
    time_t now = time(nullptr);
    const char* tempFilename = "/temp.txt";
    File input = LittleFS.open(DATA_FILENAME, "r");
    if (!input) return false;

    File output = LittleFS.open(tempFilename, "w");
    if (!output) {
        input.close();
        return false;
    }

    if(input.available()) {
        input.readStringUntil('\n');
    }
    output.print(SensorData::csvHeader());
    while(input.available()){
        String line = input.readStringUntil('\n');
        line.trim();
        int timeEnd = line.indexOf(';');
        if (timeEnd == -1) continue;
        time_t timestamp = line.substring(0, timeEnd).toInt();
        if(timestamp > now - configs.keepData) {
            output.println(line);
        }
    }
    input.close();
    output.close();

    LittleFS.remove(DATA_FILENAME);
    LittleFS.rename(tempFilename, DATA_FILENAME);
    return true;
}