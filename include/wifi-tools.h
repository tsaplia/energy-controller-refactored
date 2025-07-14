#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <ESP8266WiFi.h>
#include <DNSServer.h>

extern DNSServer dnsServer;

bool startWiFiSTA(const char* ssid, const char* password);
bool startWiFiAP();
bool disconnectAP();
void setupOTA();

#endif