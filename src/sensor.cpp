#include "sensor.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "constants.h"
#include <globals.h>

ModbusMaster node;

SensorData::SensorData(uint16_t dataArr[]) {
    valid = true;
    timestamp = time(nullptr);
    voltage = dataArr[0];
    current = (static_cast<uint32_t>(dataArr[2]) << 16) | dataArr[1];
    power = (static_cast<uint32_t>(dataArr[4]) << 16) | dataArr[3];
    energy = (static_cast<uint32_t>(dataArr[6]) << 16) | dataArr[5];
    frequency = dataArr[7];
    pf = dataArr[8];
}

SensorData::SensorData() {
    valid = false;
    timestamp = time(nullptr);
}

String SensorData::toJson() {
    if (!valid) return "{}";

    JsonDocument doc;
    doc["time"] = timestamp;
    doc["voltage"] = voltage * 0.1f;
    doc["current"] = current * 0.001f;
    doc["power"] = power * 0.1f;
    doc["energy"] = getFormattedEnergy();
    doc["frequency"] = frequency * 0.1f;
    doc["pf"] = pf * 0.01f;
    String output;
    serializeJson(doc, output);
    return output;
}

String SensorData::toCsv() {
    if (!valid) return "";
    return String(timestamp) + ";"
            +String(voltage * 0.1f) + ";" + String(current * 0.001f) + ";" 
            + String(power * 0.1f) + ";" + getFormattedEnergy() + ";" 
            + String(frequency * 0.1f) + ";" + String(pf * 0.01f) + '\n';
}

String SensorData::csvHeader() {
    return "Time;Voltage;Current;Power;Energy;Frequency;Power Factor\n";
}

String SensorData::getFormattedEnergy() {
    if (!valid) return "";
    return String((energy - configs.resetEnergyValue) * 0.001f, 1);
}

SensorData getSensorData() {
    const int regNum = 9;
    uint16_t dataArr[regNum];

    // TODO: remove
    for(int i = 0; i < regNum; ++i) {
        dataArr[i] = i+1;
    }

    return SensorData(dataArr);
    // TODO: remove end

    delay(100);
    Serial.flush();
    Serial.swap();  // switch to sensor

    uint8_t result = node.readInputRegisters(0, regNum);

    if (result != node.ku8MBSuccess) {
        Serial.flush();
        Serial.swap();  // restore Serial
        return SensorData(); // returns invalid
    } 

    for (int i = 0; i < regNum; ++i) {
        dataArr[i] = node.getResponseBuffer(i);
    }
    Serial.flush();
    Serial.swap();  // restore Serial

    return SensorData(dataArr);
}