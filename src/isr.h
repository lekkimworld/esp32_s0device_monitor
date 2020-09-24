#include "types.h"

void isr_worker(S0Device *device) {
  if ((millis() - device->lastTrigger) < DEBOUNCE_TIME) return;
  digitalWrite(device->pinLED, HIGH);
  device->startTimer = true;
  device->lastTrigger = millis();
  device->count++;
}

void IRAM_ATTR isr_plug0_orange() {
  S0Device *device = &plugs_runtime[0].devices[DEVICE_IDX_ORANGE];
  isr_worker(device);
}

void IRAM_ATTR isr_plug0_brown() {
  S0Device *device = &plugs_runtime[0].devices[DEVICE_IDX_BROWN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug0_green() {
  S0Device *device = &plugs_runtime[0].devices[DEVICE_IDX_GREEN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug0_blue() {
  S0Device *device = &plugs_runtime[0].devices[DEVICE_IDX_BLUE];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_orange() {
  S0Device *device = &plugs_runtime[1].devices[DEVICE_IDX_ORANGE];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_brown() {
  S0Device *device = &plugs_runtime[1].devices[DEVICE_IDX_BROWN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_green() {
  S0Device *device = &plugs_runtime[1].devices[DEVICE_IDX_GREEN];
  isr_worker(device);
}

void IRAM_ATTR isr_plug1_blue() {
  S0Device *device = &plugs_runtime[1].devices[DEVICE_IDX_BLUE];
  isr_worker(device);
}

void initializeISR() {
  plugs_runtime[0].devices[DEVICE_IDX_ORANGE].pinIRQ = 35;
  plugs_runtime[0].devices[DEVICE_IDX_ORANGE].pinLED = 15;
  plugs_runtime[0].devices[DEVICE_IDX_ORANGE].isr = isr_plug0_orange;
  
  plugs_runtime[0].devices[DEVICE_IDX_BROWN].pinIRQ = 34;
  plugs_runtime[0].devices[DEVICE_IDX_BROWN].pinLED = 2;
  plugs_runtime[0].devices[DEVICE_IDX_BROWN].isr = isr_plug0_brown;
  
  plugs_runtime[0].devices[DEVICE_IDX_BLUE].pinIRQ = 39;
  plugs_runtime[0].devices[DEVICE_IDX_BLUE].pinLED = 4;
  plugs_runtime[0].devices[DEVICE_IDX_BLUE].isr = isr_plug0_blue;

  plugs_runtime[0].devices[DEVICE_IDX_GREEN].pinIRQ = 36;
  plugs_runtime[0].devices[DEVICE_IDX_GREEN].pinLED = 16;
  plugs_runtime[0].devices[DEVICE_IDX_GREEN].isr = isr_plug0_green;

  plugs_runtime[1].devices[DEVICE_IDX_ORANGE].pinIRQ = 32;
  plugs_runtime[1].devices[DEVICE_IDX_ORANGE].pinLED = 17;
  plugs_runtime[1].devices[DEVICE_IDX_ORANGE].isr = isr_plug1_orange;
  
  plugs_runtime[1].devices[DEVICE_IDX_BROWN].pinIRQ = 33;
  plugs_runtime[1].devices[DEVICE_IDX_BROWN].pinLED = 5;
  plugs_runtime[1].devices[DEVICE_IDX_BROWN].isr = isr_plug1_brown;
  
  plugs_runtime[1].devices[DEVICE_IDX_BLUE].pinIRQ = 25;
  plugs_runtime[1].devices[DEVICE_IDX_BLUE].pinLED = 18;
  plugs_runtime[1].devices[DEVICE_IDX_BLUE].isr = isr_plug1_blue;

  plugs_runtime[1].devices[DEVICE_IDX_GREEN].pinIRQ = 26;
  plugs_runtime[1].devices[DEVICE_IDX_GREEN].pinLED = 19;
  plugs_runtime[1].devices[DEVICE_IDX_GREEN].isr = isr_plug1_green;
}
