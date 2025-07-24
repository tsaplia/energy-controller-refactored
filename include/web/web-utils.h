#ifndef WEB_UTILS_H
#define WEB_UTILS_H

#include <Arduino.h>

void sendCorsHeader();
void sendMessage(int code, const char* message);
void sendFile(String path);
void optionsHandler();
bool isApRequest();
bool isStaRequest();
void sendRootRedirect();
String getSocketPayload(const String& type, const String& msg, bool wrap = true);
String getContentType(const String& filename);

#endif