#include "globals.h"
#include "constants.h"

Configs configs(CONFIGS_FILENAME);
Logger logger;
unsigned long lastTimerSync = 0;
bool timeSynced = false;
