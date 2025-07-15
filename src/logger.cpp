#include "logger.h"

const String LOG_LEVEL_NAMES[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

/* Format time as "YYYY-MM-DD HH:MM:SS" for logging */
String formatTime(time_t t) {
    char buf[20];
    struct tm* tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
    return String(buf);
}

LogEntry::operator String() const {
    return formatTime(this->timestamp) + " [" + LOG_LEVEL_NAMES[this->level] + "] " + this->msg;
}

void LogStorage::add(const LogEntry& log) {
    if (log.level >= logLevel) {
        logs[firstLogPos] = log;
        firstLogPos = (firstLogPos + 1) % MAX_LOG_ENTRIES;
        if (size < MAX_LOG_ENTRIES) size++;

        if (onNewLog != nullptr) onNewLog(log);
    }
}

void LogStorage::setLogLevel(int level) {
    if (level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_ERROR) {
        this->logLevel = level;
    }
}

String LogStorage::getLogs() const {
    String output;
    int start = size < MAX_LOG_ENTRIES ? 0 : firstLogPos;
    for (int i = 0; i < size; i++) {
        output += String(logs[(start + i) % MAX_LOG_ENTRIES]) + ";";
    }
    return output;
}

void LogStorage::onNew(std::function<void(const LogEntry &)> callback) {
    onNewLog = callback;
}

void Logger::log(int level, const String& msg) {
    LogEntry log = {level, msg, time(nullptr)};
    Serial.println(log);
    logStorage.add(log);
}

void Logger::debug(const String& msg) { log(LOG_LEVEL_DEBUG, msg); }
void Logger::info(const String& msg) { log(LOG_LEVEL_INFO, msg); }
void Logger::warning(const String& msg) { log(LOG_LEVEL_WARNING, msg); }
void Logger::error(const String& msg) { log(LOG_LEVEL_ERROR, msg); }

void Logger::setLogLevel(int level) {
    logStorage.setLogLevel(level);
}
