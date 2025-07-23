
#include "utils.h"
#include <LittleFS.h>
#include <Esp.h>
#include "globals.h"
#include <wifi-tools.h>
#include <web-server.h>
#include <sensor.h>

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
  
String formatTime(time_t t, const char* format) {
    char buf[25];
    struct tm* tmInfo = localtime(&t);
    strftime(buf, sizeof(buf), format, tmInfo);
    return String(buf);
}

bool syncTime() {
    if(WiFi.status() != WL_CONNECTED) return false;

    configTime(TIMEZONE_OFFSET_SEC, 0, TIME_SERVER_1, TIME_SERVER_2);
    while (time(nullptr) < 100000) delay(100);

    appState.lastTimerSync = millis();
    appState.timeSynced = true;
    logger.info("Time updated");
    return true;
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
