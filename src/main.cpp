#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include "constants.h"
#include "web-server.h"
#include "ap-server.h"

DNSServer dnsServer;

void setupOTA();
void startWiFiAP();
bool startSTA(const char* ssid, const char* password);

void setup() {
    Serial.begin(115200);
    Serial.flush();
    Serial.println("Booting Sketch...");
    LittleFS.begin();
    startWiFiAP();

    setupApServer();
    ap_server.begin();
}

void loop() {
    dnsServer.processNextRequest();
    ap_server.handleClient();
    delay(500);
}

void startWiFiAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
    bool result = WiFi.softAP(AP_SSID, AP_PASSWORD);
    if (result) {
        Serial.print("AP started with IP: ");
        Serial.println(WiFi.softAPIP());
        dnsServer.start(DNS_PORT, "*", AP_IP);
    } else {
        Serial.println("Failed to start AP");
    }
}

bool startSTA(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    for(int tries = 0; WiFi.status() != WL_CONNECTED && tries < WIFI_CONNECT_TRIES; tries++){
        delay(WIFI_CONNECT_DELAY);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } 
    Serial.println("\nFailed to connect to WiFi");
    return false;
    
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