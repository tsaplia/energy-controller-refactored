#include "sensor.h"
#include <Arduino.h>
#include <ArduinoJson.h>

ModbusMaster node;

SensorData::SensorData(uint16_t dataArr[]) {
    valid = true;
    voltage = dataArr[0];
    current = (static_cast<uint32_t>(dataArr[2]) << 16) | dataArr[1];
    power = (static_cast<uint32_t>(dataArr[4]) << 16) | dataArr[3];
    energy = (static_cast<uint32_t>(dataArr[6]) << 16) | dataArr[5];
    frequency = dataArr[7];
    pf = dataArr[8];
}

String SensorData::toJson() {
    if (!valid) return "{}";

    JsonDocument doc;
    doc["voltage"] = voltage * 0.1f;
    doc["current"] = current * 0.001f;
    doc["power"] = power * 0.1f;
    doc["energy"] = energy;
    doc["frequency"] = frequency * 0.1f;
    doc["pf"] = pf * 0.01f;
    String output;
    serializeJson(doc, output);
    return output;
}

SensorData getSensorData() {
    const int regNum = 9;
    uint16_t dataArr[regNum];

    for(int i = 0; i < regNum; ++i) {
        dataArr[i] = i+1;
    }

    return SensorData(dataArr);

    delay(100);
    Serial.swap();  // switch to sensor
    Serial.flush();

    uint8_t result = node.readInputRegisters(0, regNum);

    if (result != node.ku8MBSuccess) {
        Serial.swap();  // restore Serial
        Serial.flush();
        return SensorData(); // returns invalid
    } 

    for (int i = 0; i < regNum; ++i) {
        dataArr[i] = node.getResponseBuffer(i);
    }
    Serial.swap();  // restore Serial
    Serial.flush();

    return SensorData(dataArr);
}


// uint8_t resetEnergy(){
//     uint8_t result;
    
//     delay(100); 
//     Serial.swap();
//     Serial.flush();
    
//     result = node.resetenergy(); 
    
//     Serial.swap();
//     Serial.flush();

//     energy_config["day"] = (uint32_t)energy_config["day"] - getSensorData().energy; 
//     energy_config["night"] = (uint32_t)energy_config["day"] - getSensorData().energy;
//     energy_config["all"] = (uint32_t)energy_config["day"] - getSensorData().energy;
//     saveOffset();
    
//     return result;
// }
