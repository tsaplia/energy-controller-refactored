#ifndef CONFIGS_H
#define CONFIGS_H

#include <Arduino.h>

class Configs {
public:
    Configs(const char* filename);

    bool load();
    bool save() const;

    String ssid;
    String password;
private:
    const char* filename;
};

#endif
