#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;
extern DeviceConfig deviceconfig;
int page = -1;
        
void updateDisplay() {
    if (!deviceconfig.useDisplay) return;
    S0_LOG_TRACE("[DISPLAY] updating");
    char buffer[48];
    
    // go to page 0 if over number of plugs
    page++;
    S0_LOG_TRACE("[DISPLAY] display page is: %d", page);
    if (page > RJ45_PLUG_COUNT) page = 0;

    // clear display
    display.clearDisplay();
    int y = 0;

    // add page no
    char page_no[16];
    sprintf(page_no, "Page %d/%d", page+1, RJ45_PLUG_COUNT+1);
    display.setCursor(75, y);
    display.println(page_no);
    y += 15;

    if (0 == page) {
        // network info page - show Mac
        display.setCursor(0, y);
        display.print("MAC:");
        char mac_addr[20];
        getMacAddressString(mac_addr);
        display.setCursor(25, y);
        display.println(mac_addr);
        y += 10;

        // show IP
        display.setCursor(0, y);
        display.print("IP :");
        display.setCursor(25, y);
        display.println(WiFi.localIP().toString().c_str());
        y += 10;

        // show endpoint
        display.setCursor(0, y);
        display.println("Endpoint:");
        y += 10;
        display.setCursor(0, y);
        display.println(deviceconfig.endpoint);
        y += 10;

    } else {
        // status for plug - get plug for page
        RJ45 plug = plugs_runtime[page - 1];

        // print plug name
        sprintf(buffer, "Plug: %s", plug.name);
        display.setCursor(0, y);
        display.println(buffer);
        y += 10;

        // loop devices on plug
        for (int i = 0; i < plug.activeDevices; i++) {
            S0Device device = plug.devices[i];
            sprintf(buffer, "%s: %d", device.name, device.count);
            display.setCursor(0, y);
            display.println(buffer);
            y += 10;
        }
    }

    // update display
    display.display();
}

void writeDisplay(char *buffer) {
    if (!deviceconfig.useDisplay) return;

    display.setCursor(0, 10);
    display.clearDisplay();
    display.println(buffer);
    display.display();
}

void initDisplay() {
    if (!deviceconfig.useDisplay) {
        S0_LOG_INFO("Not initializing display as useDisplay=false");
        return;
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        S0_LOG_ERROR("SSD1306 display allocation failed");
        for (;;)
            ;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    updateDisplay();
}