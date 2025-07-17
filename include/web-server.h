#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <WebSockets4WebServer.h>

extern ESP8266WebServer webServer;
extern WebSockets4WebServer logSocket;
extern WebSockets4WebServer dataSocket;

String getSocketPayload(const String& type, const String& msg, bool wrap = true);
void setupWebServer();

#endif