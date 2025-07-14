#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <ESP8266WiFi.h>
#include <DNSServer.h>

extern DNSServer dnsServer;
extern bool APConnected;

void startWiFiSTA(String& ssid, String& password);
bool changeWiFiSTA(String& ssid, String& password);
bool startWiFiAP();
bool disconnectAP();
void setupOTA();

#endif