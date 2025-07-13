#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include "constants.h"
#include "web-server.h"
#include "wifi-tools.h"

void setupOTA();

void setup() {
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Booting Sketch...");
    LittleFS.begin();

    WiFi.mode(WIFI_AP_STA);
    startWiFiAP();

    setupWebServer();
    webServer.begin();
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    delay(200);
}


void setupOTA() {
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
        }
    });
}