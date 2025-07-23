#include "globals.h"
#include <LittleFS.h>
#include "constants.h"

Configs configs(CONFIGS_FILENAME);
Logger logger;
FTPServer ftpSrv(LittleFS);
AppState appState;
