# esp32_s0device_monitor #


## upload including SPIFFS ##
```
scripts/inline_management_ui.sh && pio run -e dev -t upload && pio run -e dev -t uploadfs
```

## credentials.h ##
* WIFI_SSID
* WIFI_PASS
* ENDPOINT_URL
* DEVICE_JWT

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
```
S0+ --> button right --> 5v
S0- --> button left --> 4,7kOhm --> signal pin
                                --> 10kOhm --> Ground
```

## Pin map ##
* I2C
    * SDA 21
    * SCL 22
* RJ45 plug 1
    * Orange
        * ISR 35
        * LED 19
    * Brown
        * ISR 34
        * LED 18
    * Blue  
        * ISR 39
        * LED 5
    * Green
        * ISR 36
        * LED 17
* RJ45 plug 2
    * Orange
        * ISR 32
        * LED 16
    * Brown
        * ISR 33
        * LED 4
    * Green
        * ISR 25
        * LED 2
    * Blue  
        * ISR 26
        * LED 15
* Buttons
    * Menu up
        * IRQ 27
    * Menu down
        * IRQ 14
    * Menu up
        * IRQ 12
    * Menu up
        * IRQ 13
* Free
    * Right
        * 1
        * 3
        * 23

## Dependencies ##
ArduinoLog
https://github.com/thijse/Arduino-Log

 Log.trace(F("Configuring plugs and devices"));
    strcpy(plugs[0].name, "Yellow RJ45");
    strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].name, "tumbler (Or)   : %d");
    strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].id,   "s0dryer");
    strcpy(plugs[0].devices[DEVICE_IDX_BROWN].name,  "Vaskemask. (Br): %d");
    strcpy(plugs[0].devices[DEVICE_IDX_BROWN].id,    "s0washer");
    strcpy(plugs[0].devices[DEVICE_IDX_GREEN].name,  "Opvask. (Gr)   : %d");
    strcpy(plugs[0].devices[DEVICE_IDX_GREEN].id,    "s0dishwasher");
    strcpy(plugs[0].devices[DEVICE_IDX_BLUE].name,   "Varmepumpe (Bl): %d");
    strcpy(plugs[0].devices[DEVICE_IDX_BLUE].id,     "s0heatpump");
    plugs[0].activeDevices = 4;

    strcpy(plugs[1].name, "White RJ45");
    strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].id,   "s0floorbath");
    strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].name, "Bad, gulvvarme : %d");
    plugs[1].activeDevices = 1;
