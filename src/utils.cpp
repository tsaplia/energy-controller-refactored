
#include "utils.h"
#include <LittleFS.h>
#include <Esp.h>
#include "globals.h"

void logSystemInfo() {
    logger.info("== Chip Info ==");
    logger.info("Chip ID: " + String(ESP.getChipId()));
    logger.info("Core version: " + String(ESP.getCoreVersion()));
    logger.info("SDK version: " + String(ESP.getSdkVersion()));
    logger.info("CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz");

    logger.info("== Flash Info ==");
    logger.info("Flash chip size: " + String(ESP.getFlashChipSize() / 1024) + " KB");
    logger.info("Flash chip speed: " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz");
    logger.info("Sketch size: " + String(ESP.getSketchSize() / 1024) + " KB");
    logger.info("Free sketch space: " + String(ESP.getFreeSketchSpace() / 1024) + " KB");

    logger.info("== RAM Info ==");
    logger.info("Free heap: " + String(ESP.getFreeHeap() / 1024.0, 2) + " KB");
    logger.info("Max free block size: " + String(ESP.getMaxFreeBlockSize() / 1024.0, 2) + " KB");
    logger.info("Heap fragmentation: " + String(ESP.getHeapFragmentation()) + " %");

    logger.info("== Filesystem Info ==");
    FSInfo fsInfo;
    LittleFS.info(fsInfo);
    float totalKB = fsInfo.totalBytes / 1024.0;
    float usedKB = fsInfo.usedBytes / 1024.0;
    logger.info("Total space: " + String(totalKB, 2) + " KB");
    logger.info("Used space: " + String(usedKB, 2) + " KB");
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