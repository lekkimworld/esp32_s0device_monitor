#pragma once
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "deviceconfig.h"
#include "types.h"

RJ45 plugs[2];
WifiConfig wifiCfg;
DeviceConfig deviceCfg;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool useDisplay = true;
unsigned long lastPageChange = 0;
int page = 0;
ConfigWebServer *server;
unsigned long now = millis();
unsigned long samplePeriodStart = 0;
bool justReset = true;
bool softAPEnabled = false;
