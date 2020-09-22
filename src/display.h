
void updateDisplay() {
    if (!useDisplay) return;
    char buffer[48];

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
        display.println("Mac address:");
        y += 10;
        char mac_addr[20];
        getMacAddressString(mac_addr);
        display.setCursor(0, y);
        display.println(mac_addr);
        y += 10;

        // show IP
        display.setCursor(0, y);
        display.println("IP address:");
        y += 10;
        display.setCursor(0, y);
        display.println(WiFi.localIP().toString().c_str());
        y += 10;

    } else {
        // status for plug - get plug for page
        RJ45 plug = plugs[page - 1];

        // print plug name
        sprintf(buffer, "Plug: %s", plug.name);
        display.setCursor(0, y);
        display.println(buffer);
        y += 10;

        // loop devices on plug
        for (int i = 0; i < plug.activeDevices; i++) {
            S0Device device = plug.devices[i];
            sprintf(buffer, device.name, device.count);
            display.setCursor(0, y);
            display.println(buffer);
            y += 10;
        }
    }

    // update display
    display.display();
}

void writeDisplay(char *buffer) {
    if (!useDisplay) return;

    display.setCursor(0, 10);
    display.clearDisplay();
    display.println(buffer);
    display.display();
}

void initDisplay() {
    if (!useDisplay) {
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