#include <ArduinoOTA.h>
#include "wifi-tools.h"
#include "constants.h"

DNSServer dnsServer;

/* Start an Access Point */
bool startWiFiAP() {
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
    bool result = WiFi.softAP(AP_SSID, AP_PASSWORD);
    if (result) {
        Serial.print("AP started with IP: ");
        Serial.println(WiFi.softAPIP());
        dnsServer.start(DNS_PORT, "*", AP_IP);
    } else {
        Serial.println("Failed to start AP");
    }
    return result;
}

/* Connect to a WiFi network */
bool startWiFiSTA(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    for(int tries = 0; WiFi.status() != WL_CONNECTED && tries < WIFI_CONNECT_TRIES; tries++){
        delay(WIFI_CONNECT_DELAY);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected on Ip: " + WiFi.localIP().toString());
        ArduinoOTA.begin();
        Serial.println("OTA started");
        delay(500);
        return true;
    } 
    Serial.println("Failed to connect to WiFi");
    return false;
}

/* Disconnect from an Access Point */
bool disconnectAP() {
    Serial.println("Disconnecting Access Point");
    dnsServer.stop();
    bool result = WiFi.softAPdisconnect(true);
    if(!result) Serial.println("Failed to disconnect from Access Point");
    return result;
}

/* Setup Arduino OTA  for WiFi firmware updates */
void setupOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPort(OTA_PORT);

    ArduinoOTA.onEnd([]() {
        Serial.println("OTA End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA Error[%u]: ", error);
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
    ArduinoOTA.onStart([]() { 
        Serial.println("OTA Start"); 
    });
}