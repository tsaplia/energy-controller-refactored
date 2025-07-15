#include "globals.h"
#include "constants.h"

Configs configs(CONFIGS_FILENAME);
Logger logger;

unsigned long lastTimerSync = -TIME_SYNC_INTERVAL;
