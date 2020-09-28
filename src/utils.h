#pragma once
#include <WiFi.h>

/**
 * Get Mac address to use.
 */
void getMacAddress(byte *mac) {
    WiFi.macAddress(mac);
}

/**
 * Convert mac address to a char buffer.
 */
void getMacAddressString(char *buffer) {
    byte mac[6];
    getMacAddress(mac);
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * Convert mac address to a char buffer.
 */
void getMacAddressStringNoColon(char *buffer) {
    byte mac[6];
    getMacAddress(mac);
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void buildNetworkName(char* buffer) {
  char mac_addr[18];
  getMacAddressStringNoColon(mac_addr);
  sprintf(buffer, "S0Monitor-%s", mac_addr);
}

bool hasWebEndpoint() {
    return strlen(deviceconfig.endpoint) > 0;
}
