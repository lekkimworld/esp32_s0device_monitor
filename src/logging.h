#include <Arduino.h>
#include <Syslog.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_ERROR 3

extern Syslog syslog;
bool hasSyslog();

template <typename... Args>
void S0_LOG_TRACE(const char *msg, Args... args) {
#ifndef DISABLE_LOGGING
    if (LOG_LEVEL_TRACE >= S0_LOG_LEVEL) {
        Serial.print("[TRACE] - ");
        Serial.printf(msg, args...);
        Serial.println();
    }
#endif
}

template <typename... Args>
void S0_LOG_DEBUG(const char *msg, Args... args) {
#ifndef DISABLE_LOGGING
    if (LOG_LEVEL_DEBUG >= S0_LOG_LEVEL) {
        Serial.print("[DEBUG] - ");
        Serial.printf(msg, args...);
        Serial.println();
        if (hasSyslog()) syslog.logf(LOG_KERN, msg, args...);
    }
#endif
}

template <typename... Args>
void S0_LOG_INFO(const char *msg, Args... args) {
#ifndef DISABLE_LOGGING
    if (LOG_LEVEL_INFO >= S0_LOG_LEVEL) {
        Serial.print(" [INFO] - ");
        Serial.printf(msg, args...);
        Serial.println();
        if (hasSyslog()) syslog.logf(LOG_INFO, msg, args...);
    }
#endif
}

template <typename... Args>
void S0_LOG_ERROR(const char *msg, Args... args) {
#ifndef DISABLE_LOGGING
    if (LOG_LEVEL_ERROR >= S0_LOG_LEVEL) {
        Serial.print("[ERROR] - ");
        Serial.printf(msg, args...);
        Serial.println();
        if (hasSyslog()) syslog.logf(LOG_ERR, msg, args...);
    }
#endif
}
