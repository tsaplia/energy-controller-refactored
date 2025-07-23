#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "constants.h"

constexpr int LOG_LEVEL_DEBUG = 0;
constexpr int LOG_LEVEL_INFO = 1;
constexpr int LOG_LEVEL_WARNING = 2;
constexpr int LOG_LEVEL_ERROR = 3;

extern const String LOG_LEVEL_NAMES[];

struct LogEntry {
    int level;
    String msg;
    time_t timestamp;

    operator String() const;
    String toFileFormat() const;
};

class LogStorage {
private:
    bool saved = true;
    int logLevel = LOG_LEVEL_INFO;
    int firstLogPos = 0;
    int size = 0;
    LogEntry logs[MAX_LOG_ENTRIES];
    
    std::function<void(const LogEntry&)> onNewLog = nullptr; 
    
public:
    void add(const LogEntry& log);
    bool setLogLevel(int level);
    String getLogs() const;
    void onNew(std::function<void(const LogEntry&)> callback);
    bool load(const char* filename);
    bool save(const char* filename);
    bool isSaved() const;
};

class Logger {
private:
    void log(int level, const String& msg);

public:
    LogStorage logStorage;
    void debug(const String& msg);
    void info(const String& msg);
    void warning(const String& msg);
    void error(const String& msg);
    void setLogLevel(int level);
};

#endif