#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "isr.h"

// declaration
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long now = millis();
unsigned long lastPageChange = 0;
int page = 0;

void updateDisplay() {
  char buffer[24];
  display.clearDisplay();
  int y = 10;
  
  // get plug for page
  RJ45 plug = plugs[page];

  // print header
  display.setCursor(0, y);
  display.println(plug.name);
  y += 10;

  // loop devices on plug
  for (int i=0; i<plug.activeDevices; i++) {
    S0Device device = plug.devices[i];
    sprintf(buffer, device.name, device.count);
    display.setCursor(0, y);
    display.println(buffer);
    y += 10;
  }
  display.display(); 
}

void initDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  updateDisplay();
}

void initS0Pins() {
  for (int i=0, j=sizeof(plugs)/sizeof(RJ45); i<j; i++) {
    // get next plug
    RJ45 plug = plugs[i];

    // loop devices on plug
    for (int k=0; k<plug.activeDevices; k++) {
      // get next device
      S0Device device = plug.devices[k];
      
      // init input pins and attach interrupt rutines
      pinMode(device.pinIRQ, INPUT);
      attachInterrupt(digitalPinToInterrupt(device.pinIRQ), device.isr, RISING);

      // ensure LED is not on
      pinMode(device.pinLED, OUTPUT);
      digitalWrite(device.pinLED, LOW);
    }
  }
}

void turnOffLed(S0Device *device) {
  if (device->startTimer && (now - device->lastTrigger > LED_BLINK_TIME)) {
    digitalWrite(device->pinLED, LOW);
    device->startTimer = false;
  }
}

void setup() {
  WiFi.begin("MM", "chippo-lekkim");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi...");
  }

  // setup plugs
  strcpy(plugs[0].name, "Primary RJ45");
  plugs[0].activeDevices = 4;
  strcpy(plugs[1].name, "Secondary RJ45");
  plugs[1].activeDevices = 0;
  
  // setup connections per plug
  plugs[0].devices[DEVICE_IDX_ORANGE].pinIRQ = 32;
  plugs[0].devices[DEVICE_IDX_ORANGE].pinLED = 26;
  plugs[0].devices[DEVICE_IDX_ORANGE].isr = isr_plug0_orange;
  strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].name, "Or (tumbler)   : %d");
  plugs[0].devices[DEVICE_IDX_BROWN].pinIRQ = 35;
  plugs[0].devices[DEVICE_IDX_BROWN].pinLED = 27;
  plugs[0].devices[DEVICE_IDX_BROWN].isr = isr_plug0_brown;
  strcpy(plugs[0].devices[DEVICE_IDX_BROWN].name,  "Br (vaskemask.): %d");
  plugs[0].devices[DEVICE_IDX_GREEN].pinIRQ = 34;
  plugs[0].devices[DEVICE_IDX_GREEN].pinLED = 14;
  plugs[0].devices[DEVICE_IDX_GREEN].isr = isr_plug0_green;
  strcpy(plugs[0].devices[DEVICE_IDX_GREEN].name,  "Gr (opvask.)   : %d");
  plugs[0].devices[DEVICE_IDX_BLUE].pinIRQ = 33;
  plugs[0].devices[DEVICE_IDX_BLUE].pinLED = 12;
  plugs[0].devices[DEVICE_IDX_BLUE].isr = isr_plug0_blue;
  strcpy(plugs[0].devices[DEVICE_IDX_BLUE].name,   "Bl (varmepumpe): %d");

  plugs[1].devices[DEVICE_IDX_ORANGE].pinIRQ = 33;
  plugs[1].devices[DEVICE_IDX_ORANGE].pinLED = 12;
  plugs[1].devices[DEVICE_IDX_ORANGE].isr = isr_plug1_blue;
  strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].name,   "Foo         : %d");
  
  // setup serial and init display
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 display allocation failed"));
    for(;;);
  }
  initDisplay();

  // init S0 pins
  initS0Pins();
}

void loop() {
  // update current time
  now = millis();
  boolean shouldUpdateDisplay = false;
  
  // loop plugs
  for (int i=0, j=sizeof(plugs)/sizeof(RJ45); i<j; i++) {
    // get next plug
    RJ45 plug = plugs[i];

    // loop devices on plug
    for (int k=0; k<plug.activeDevices; k++) {
      // get next device
      S0Device *device = &plug.devices[k];

      // turn of LED if time has elapsed
      turnOffLed(device);
    }
  }

  // calculate page to show
  if (now - lastPageChange > PAGE_DISPLAY_TIME) {
    shouldUpdateDisplay = true;
    page++;
    if (plugs[1].activeDevices ==0 || page >= sizeof(plugs)/sizeof(RJ45)) page = 0;
    lastPageChange = now;
  }

  // update display if appropriate
  if (shouldUpdateDisplay) updateDisplay();
}
