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