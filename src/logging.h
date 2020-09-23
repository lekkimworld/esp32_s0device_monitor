#include <Arduino.h>
#include <Syslog.h>

extern Syslog syslog;
bool hasSyslog();

template <typename... Args>
void S0_LOG_TRACE(const char *msg, Args... args) {
    /*
    Serial.print("[TRACE] - ");
    Serial.printf(msg, args...);
    Serial.println();
    */
}

template <typename... Args>
void S0_LOG_DEBUG(const char *msg, Args... args) {
    Serial.print("[DEBUG] - ");
    Serial.printf(msg, args...);
    Serial.println();
    if (hasSyslog()) syslog.logf(LOG_KERN, msg, args...);
}

template <typename... Args>
void S0_LOG_INFO(const char *msg, Args... args) {
    Serial.print(" [INFO] - ");
    Serial.printf(msg, args...);
    Serial.println();
    if (hasSyslog()) syslog.logf(LOG_INFO, msg, args...);
}

template <typename... Args>
void S0_LOG_ERROR(const char *msg, Args... args) {
    Serial.print("[ERROR] - ");
    Serial.printf(msg, args...);
    Serial.println();
    if (hasSyslog()) syslog.logf(LOG_ERR, msg, args...);
}
