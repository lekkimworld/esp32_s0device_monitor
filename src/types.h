#define LED_BLINK_TIME 200
#define DEBOUNCE_TIME 200
#define PAGE_DISPLAY_TIME 2000
#define DEVICE_IDX_ORANGE 0
#define DEVICE_IDX_BROWN  1
#define DEVICE_IDX_BLUE   2
#define DEVICE_IDX_GREEN  3

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

typedef struct {
  int pinIRQ;
  int pinLED;
  unsigned long lastTrigger = 0;
  bool startTimer = false;
  volatile unsigned int count = 0;
  char name[24];
  void (*isr)();
} S0Device;

typedef struct {
  char name[24];
  int activeDevices = 0;
  S0Device devices[4];
} RJ45;
