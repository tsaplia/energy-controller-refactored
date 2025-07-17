#ifndef UTILS_H
#define UTILS_H

#include <WString.h>
#include <time.h>

String getContentType(const String& filename);
String formatTime(time_t t, const char* format);
void logSystemInfo();
bool syncTime();

#endif