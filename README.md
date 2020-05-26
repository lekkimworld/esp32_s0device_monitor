# esp32_s0device_monitor #


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
        * ISR 36
        * LED 19
    * Brown
        * ISR 39
        * LED 18
    * Blue  
        * ISR 34
        * LED 5
    * Green
        * ISR 35
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
