#ifndef GLOBALS_H
#define GLOBALS_H

#include <ModbusMaster.h>
#include "configs.h"
#include "logger.h"

extern Configs configs;
extern Logger logger;
extern unsigned long lastTimerSync;
extern bool timeSynced;
extern bool running;

#endif