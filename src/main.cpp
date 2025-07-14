#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LIttleFS.h>
#include <FTPServer.h>
#include "constants.h"
#include "web-server.h"
#include "wifi-tools.h"
#include "configs.h"
#include "globals.h"

FTPServer ftpSrv(LittleFS);

void logFsInfo() {
    FSInfo fsInfo;
    LittleFS.info(fsInfo);

    Serial.print("Total space: ");
    Serial.print(fsInfo.totalBytes / 1024.0, 2);
    Serial.println(" KB");

    Serial.print("Used space: ");
    Serial.print(fsInfo.usedBytes / 1024.0, 2);
    Serial.println(" KB");

    Serial.print("Free space: ");
    Serial.print((fsInfo.totalBytes - fsInfo.usedBytes) / 1024.0, 2);
    Serial.println(" KB");
}

void setup() {
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Booting Sketch...");

    LittleFS.begin();
    Serial.println("LittleFS mounted");

    logFsInfo();
    configs.load();

    
    WiFi.mode(WIFI_AP_STA);
    if(configs.ssid.length() > 0 && configs.password.length() > 0) {
        startWiFiSTA(configs.ssid, configs.password);
    }
    
    setupWebServer();
    webServer.begin();
    Serial.println("Web server started");

    setupOTA();
    ArduinoOTA.begin();
    Serial.println("OTA started");

    ftpSrv.begin(FTP_USER, FTP_PASSWORD);
    Serial.println("FTP server started");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();

    if(WiFi.status() == WL_CONNECTED && APConnected) {
        Serial.println("STA and AP are connected at the same time");
        disconnectAP();
    } else if (WiFi.status() != WL_CONNECTED && !APConnected) {
        Serial.println("No connection to WiFi, starting AP");
        startWiFiAP();
    }
    delay(200);
}