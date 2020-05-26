#include <Arduino.h>
#define CONFIGURATION_VERSION 1

// define struct to hold general config
struct {
  uint8_t version = CONFIGURATION_VERSION;
  char endpoint[64] = "";
  char jwt[512] = "";
  unsigned long delayPost = 0L;
} configuration;

// define struct to hold wifi configuration
struct { 
    char ssid[20] = "";
    char password[20] = "";
    bool keep_ap_on = false;
} wifi_data;

void initWebserver();
