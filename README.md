# esp32_s0device_monitor #


## upload including SPIFFS ##
```
pio run -e dev -t upload && pio run -e dev -t uploadfs
```

## RJ45 wiring ##
T-568B: https://www.bittree.com/blogs/get-connected/rj45-pinout
* Pin 1: (orange/white) S0-
* Pin 2: (orange) S0+
* Pin 3: (green/white) S0-
* Pin 4: (blue) S0+
* Pin 5: (blue/white) S0-
* Pin 6: (green) S0+
* Pin 7: (brown/white) S0-
* Pin 8: (brown) S0+

### RJ45 plug pinout ###
*Bottom up*
```
     O      O
  2o  4o  6o  8o
1o  3o  5o  7o
```
*Top down*
```
     O      O
  7o  5o  3o  1o
8o  6o  4o  2o
```

### Wire colors ###
* P0D0 = orange
* P0D1 = brown
* P0D2 = blue
* P0D3 = green
* P1D0 = orange
* P1D1 = brown
* P1D2 = blue
* P1D3 = green

### ESP32 on PCB pinout ###
```
           ANT
      V3 -    - 
         -    - 
BTN P0D3 -    -
BTN P0D2 -    -
BTN P0D1 -    -
BTN P0D0 -    -
BTN P1D0 -    -
BTN P1D1 -    - P1D3 LED
BTN P1D2 -    - P1D2 LED
BTN P1D3 -    - P1D1 LED
         -    - P1D0 LED
         -    - P0D3 LED
         -    - P0D2 LED
         -    - 
         -    - P0D1 LED
         -    - P0D0 LED
         -    - 
         -    - 
         -    - 
           USB
```
