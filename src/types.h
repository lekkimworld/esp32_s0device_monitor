#pragma once
#define RJ45_PLUG_COUNT 2
#define LED_BLINK_TIME 200
#define DEBOUNCE_TIME 200
#define PAGE_DISPLAY_TIME 2000
#define DEVICE_IDX_ORANGE 0
#define DEVICE_IDX_BROWN  1
#define DEVICE_IDX_BLUE   2
#define DEVICE_IDX_GREEN  3
#define DEVICES_PER_PLUG  4

#define DELAY_SERVER_PING 5000
#define S0_DEVICE_HOSTNAME "s0device_pcb"
#define S0_APP_NAME "s0device_pcb"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define DELAY_TURNOFF_AP 300000L        // delay after restart before turning off access point, in milliseconds

typedef struct {
  char name[20];
  char id[36];
} S0Config;

typedef struct {
  char name[20];
} RJ45Config;

typedef struct {
  int pinIRQ;
  int pinLED;
  unsigned long lastTrigger = 0;
  bool startTimer = false;
  volatile unsigned int count = 0;
  char name[20];
  char id[36];
  void (*isr)();
} S0Device;

typedef struct {
  char name[20];
  int activeDevices = 0;
  S0Device devices[4];
} RJ45;

// define struct to hold general config
#define CONFIGURATION_VERSION 2
typedef struct {
  uint8_t version = CONFIGURATION_VERSION;
  bool productionCert = false;
  bool useDisplay = true;
  char endpoint[64] = "";
  char jwt[512] = "";
  unsigned long delay_post = 0L;
  char syslog_server[512] = "";
  uint16_t syslog_port = 514;

} DeviceConfig;

// define struct to hold wifi configuration
typedef struct { 
    char ssid[20] = "";
    char password[20] = "";
    bool keep_ap_on = false;
} WifiConfig;
