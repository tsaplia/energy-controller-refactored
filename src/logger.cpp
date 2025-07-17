#include "logger.h"
#include <LittleFS.h>
#include "utils.h"

const String LOG_LEVEL_NAMES[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

LogEntry::operator String() const {
    return formatTime(this->timestamp, LOGGER_TIME_FORMAT) + " [" + LOG_LEVEL_NAMES[this->level] + "] " + this->msg;
}

String LogEntry::toFileFormat() const {
    return String(this->timestamp) + " " + String(this->level) + " " + this->msg;
}

void LogStorage::add(const LogEntry& log) {
    if (STREAM_ALL_LOGS && onNewLog != nullptr) onNewLog(log);
    if (log.level >= logLevel) {
        if(!STREAM_ALL_LOGS && onNewLog != nullptr) onNewLog(log);
        logs[firstLogPos] = log;
        firstLogPos = (firstLogPos + 1) % MAX_LOG_ENTRIES;
        if (size < MAX_LOG_ENTRIES) size++;

        saved = false;
    }
}

void LogStorage::setLogLevel(int level) {
    if (level >= LOG_LEVEL_INFO && level <= LOG_LEVEL_ERROR) {
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

LogEntry readLog(File file) {
    time_t timestamp = file.parseInt();
    int level = file.parseInt();
    String msg = file.readStringUntil(';');
    if(msg.length() == 0 || timestamp < 0 || level < LOG_LEVEL_DEBUG || level > LOG_LEVEL_ERROR) {
        return {-1, "Invalid log", 0};
    }
    return {level, msg, timestamp};
}

bool LogStorage::load(const char* filename) {
    File file = LittleFS.open(filename, "r");
    if (!file) return false;
    while (file.available()) {
        LogEntry log = readLog(file);
        if(log.level == -1) {
            this->add({LOG_LEVEL_ERROR, "Got invalid log in file", time(nullptr)});
            file.close();
            return false;
        }
        if(log.level) this->add(log);
    }
    file.close();
    return true;
}

bool LogStorage::save(const char* filename) {
    File file = LittleFS.open(filename, "w");
    if (!file) return false;
    int start = size < MAX_LOG_ENTRIES ? 0 : firstLogPos;
    for (int i = 0; i < size; i++) {
        file.print(logs[(start + i) % MAX_LOG_ENTRIES].toFileFormat() + ";");
    }
    file.close();
    saved = true;
    return true;
}

bool LogStorage::isSaved() const {
    return saved;
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
