#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <ModbusMaster.h>
#include <cstdint>
#include <time.h>

struct SensorData {
    bool valid = false;
    time_t timestamp;
    uint16_t voltage;
    uint16_t frequency;
    uint16_t pf;
    uint32_t current;
    uint32_t power;
    uint32_t energy;

    SensorData();
    SensorData(uint16_t dataArr[]);
    String toJson();
    String toCsv();
    String getFormattedEnergy();
    static String csvHeader();
};

extern ModbusMaster node;

SensorData getSensorData();

#endif
