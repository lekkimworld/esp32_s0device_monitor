#include <Arduino.h>
#include "types.h"

RJ45 plugs[2];

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

void initISR() {
  plugs[0].devices[DEVICE_IDX_ORANGE].pinIRQ = 32;
  plugs[0].devices[DEVICE_IDX_ORANGE].pinLED = 26;
  plugs[0].devices[DEVICE_IDX_ORANGE].isr = isr_plug0_orange;
  
  plugs[0].devices[DEVICE_IDX_BROWN].pinIRQ = 35;
  plugs[0].devices[DEVICE_IDX_BROWN].pinLED = 27;
  plugs[0].devices[DEVICE_IDX_BROWN].isr = isr_plug0_brown;
  
  plugs[0].devices[DEVICE_IDX_GREEN].pinIRQ = 34;
  plugs[0].devices[DEVICE_IDX_GREEN].pinLED = 14;
  plugs[0].devices[DEVICE_IDX_GREEN].isr = isr_plug0_green;
  
  plugs[0].devices[DEVICE_IDX_BLUE].pinIRQ = 33;
  plugs[0].devices[DEVICE_IDX_BLUE].pinLED = 12;
  plugs[0].devices[DEVICE_IDX_BLUE].isr = isr_plug0_blue;

  plugs[1].devices[DEVICE_IDX_ORANGE].pinIRQ = 33;
  plugs[1].devices[DEVICE_IDX_ORANGE].pinLED = 12;
  plugs[1].devices[DEVICE_IDX_ORANGE].isr = isr_plug1_blue;
}
