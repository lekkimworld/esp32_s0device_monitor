#pragma once
#include <Syslog.h>
#include "types.h"

// Create a new syslog instance with LOG_KERN facility
WiFiUDP udpClient;
Syslog syslog(udpClient, deviceCfg.syslog_server, deviceCfg.syslog_port, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
bool hasSyslog() {
    return strlen(deviceCfg.syslog_server) > 0;
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
