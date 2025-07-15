#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <WebSockets4WebServer.h>

extern ESP8266WebServer webServer;
extern WebSockets4WebServer logSocket;

void setupWebServer();

#endif