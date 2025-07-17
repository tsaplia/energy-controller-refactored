#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <ModbusMaster.h>
#include <cstdint>

struct SensorData {
    bool valid = false;
    uint16_t voltage;
    uint16_t frequency;
    uint16_t pf;
    uint32_t current;
    uint32_t power;
    uint32_t energy;

    SensorData() = default;
    SensorData(uint16_t dataArr[]);
    String toJson();
};

extern ModbusMaster node;

SensorData getSensorData();

#endif
