#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LIttleFS.h>
#include <FTPServer.h>
#include "constants.h"
#include "web-server.h"
#include "wifi-tools.h"

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
    
    WiFi.mode(WIFI_AP_STA);
    startWiFiAP();
    
    setupWebServer();
    webServer.begin();
    Serial.println("Web server started");

    ftpSrv.begin(FTP_USER, FTP_PASSWORD);
    Serial.println("FTP server started");

    setupOTA();
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();
    delay(200);
}