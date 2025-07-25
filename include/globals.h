#ifndef GLOBALS_H
#define GLOBALS_H

#include <ModbusMaster.h>
#include <FTPServer.h>
#include "configs.h"
#include "logger.h"

struct AppState {
    bool timeSynced = false;
    bool running = true;

    unsigned long lastTimerSync = 0;
    unsigned long lastLogSave = 0;
    time_t lastDataGot = 0;  // should be multiple of dataSaveInterval
    time_t lastRestart = 0; 
};

extern Configs configs;
extern Logger logger;
extern FTPServer ftpSrv;
extern AppState appState;

#endif