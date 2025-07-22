#ifndef CONFIGS_H
#define CONFIGS_H

#include <Arduino.h>

class Configs {
public:
    Configs(const char* filename);

    bool load();
    bool save() const;

    /* Wifi */
    String ssid;
    String password;
    /* Sensor*/
    uint32_t resetEnergyValue;  // energy value when last reset
    time_t lastSaveDay;         // time when last day data saved (07:00)
    time_t lastSaveNight;       // time when last night data saved (23:00)

private:
    const char* filename;
};

#endif
