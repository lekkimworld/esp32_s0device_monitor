#include "types.h"

void isr_worker(S0Device *device) {
  if ((millis() - device->lastTrigger) < DEBOUNCE_TIME) return;
  digitalWrite(device->pinLED, HIGH);
  device->startTimer = true;
  device->lastTrigger = millis();
  device->count++;
}

void IRAM_ATTR isr_plug0_orange() {
  S0Device *device = &plugs[0].devices[DEVICE_IDX_ORANGE];
  isr_worker(device);
}

void IRAM_ATTR isr_plug0_brown() {
  S0Device *device = &plugs[0].devices[DEVICE_IDX_BROWN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug0_green() {
  S0Device *device = &plugs[0].devices[DEVICE_IDX_GREEN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug0_blue() {
  S0Device *device = &plugs[0].devices[DEVICE_IDX_BLUE];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_orange() {
  S0Device *device = &plugs[1].devices[DEVICE_IDX_ORANGE];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_brown() {
  S0Device *device = &plugs[1].devices[DEVICE_IDX_BROWN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_green() {
  S0Device *device = &plugs[1].devices[DEVICE_IDX_GREEN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_blue() {
  S0Device *device = &plugs[1].devices[DEVICE_IDX_BLUE];
  isr_worker(device);
}

void initializeISR() {
  plugs[0].devices[DEVICE_IDX_ORANGE].pinIRQ = 35;
  plugs[0].devices[DEVICE_IDX_ORANGE].pinLED = 15;
  plugs[0].devices[DEVICE_IDX_ORANGE].isr = isr_plug0_orange;
  
  plugs[0].devices[DEVICE_IDX_BROWN].pinIRQ = 34;
  plugs[0].devices[DEVICE_IDX_BROWN].pinLED = 2;
  plugs[0].devices[DEVICE_IDX_BROWN].isr = isr_plug0_brown;
  
  plugs[0].devices[DEVICE_IDX_BLUE].pinIRQ = 39;
  plugs[0].devices[DEVICE_IDX_BLUE].pinLED = 4;
  plugs[0].devices[DEVICE_IDX_BLUE].isr = isr_plug0_blue;

  plugs[0].devices[DEVICE_IDX_GREEN].pinIRQ = 36;
  plugs[0].devices[DEVICE_IDX_GREEN].pinLED = 16;
  plugs[0].devices[DEVICE_IDX_GREEN].isr = isr_plug0_green;

  plugs[1].devices[DEVICE_IDX_ORANGE].pinIRQ = 32;
  plugs[1].devices[DEVICE_IDX_ORANGE].pinLED = 17;
  plugs[1].devices[DEVICE_IDX_ORANGE].isr = isr_plug1_orange;
  
  plugs[1].devices[DEVICE_IDX_BROWN].pinIRQ = 33;
  plugs[1].devices[DEVICE_IDX_BROWN].pinLED = 5;
  plugs[1].devices[DEVICE_IDX_BROWN].isr = isr_plug1_brown;
  
  plugs[1].devices[DEVICE_IDX_BLUE].pinIRQ = 25;
  plugs[1].devices[DEVICE_IDX_BLUE].pinLED = 18;
  plugs[1].devices[DEVICE_IDX_BLUE].isr = isr_plug1_blue;

  plugs[1].devices[DEVICE_IDX_GREEN].pinIRQ = 26;
  plugs[1].devices[DEVICE_IDX_GREEN].pinLED = 19;
  plugs[1].devices[DEVICE_IDX_GREEN].isr = isr_plug1_green;
}
