#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoLog.h>
#include <EEPROM.h>

#include "credentials.h"
#include "isr.h"
#include "deviceconfig.h"

// Let's Encrypt, SRG Root X1 (self-signed), https://letsencrypt.org/certs/isrgrootx1.pem.txt
const char *rootCACertificate_LetsEncrypt =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n";
// DigiCert High Assurance EV Root CA, https://www.digicert.com/digicert-root-certificates.htm
const char *rootCACertificate_Digicert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
    "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
    "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
    "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
    "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
    "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
    "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
    "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
    "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
    "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
    "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
    "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
    "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
    "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
    "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
    "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
    "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
    "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
    "+OkuE6N36B9K\n"
    "-----END CERTIFICATE-----\n";
const char *rootCACertificate = rootCACertificate_Digicert;

// declaration
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long now = millis();
unsigned long lastPageChange = 0;
int page = 0;
unsigned long SAMPLE_DURATION = 2 * 60 * 1000;
unsigned long samplePeriodStart = 0;
bool justReset = true;
WifiConfig wifiCfg;
DeviceConfig deviceCfg;
ConfigWebServer *server;

/**
 * Reads the configuration from the flash and returns 1 if successful and 
 * 0 otherwise.
 */
int readConfiguration() {
    EEPROM.begin(
        sizeof(WifiConfig) + 
        sizeof(DeviceConfig) + 
        2 * sizeof(RJ45Config) + 
        8 * sizeof(S0Config)
    );
    int offset = 0;

    // read device config
    EEPROM.get(offset, deviceCfg);
    offset += sizeof(DeviceConfig);

    // validate config
    if (deviceCfg.version != CONFIGURATION_VERSION) {
        // not a valid config
        Log.trace(F("Couldn't find device config of required version (%d) in flash" CR), CONFIGURATION_VERSION);
        return false;
    }
    Log.trace(F("Read DEVICE configuration from flash - data follows" CR));
    Log.trace(F("Endpoint  : %s" CR), deviceCfg.endpoint);
    Log.trace(F("JWT       : %s" CR), deviceCfg.jwt);
    Log.trace(F("Delay post: %l" CR), deviceCfg.delay_post);

    // read wifi config
    EEPROM.get(offset, wifiCfg);
    offset += sizeof(WifiConfig);
    Log.trace(F("Read WIFI configuration from flash - data follows" CR));
    Log.trace(F("SSID      : %s" CR), wifiCfg.ssid);
    Log.trace(F("Password  : %s" CR), wifiCfg.password);
    Log.trace(F("Keep AP on: %d" CR), wifiCfg.keep_ap_on);

    // read config for first rj45 and then devices
    RJ45Config rj45cfg_0;
    S0Config s0config_0[4];
    EEPROM.get(offset, rj45cfg_0);
    offset += sizeof(RJ45Config);
    for (uint8_t i=0; i<4; i++) {
        EEPROM.get(offset, s0config_0[i]);
        offset += sizeof(S0Config);
    }

    // read config for second rj45 and then devices
    RJ45Config rj45cfg_1;
    S0Config s0config_1[4];
    EEPROM.get(offset, rj45cfg_1);
    offset += sizeof(RJ45Config);
    for (uint8_t i=0; i<4; i++) {
        EEPROM.get(offset, s0config_1[i]);
        offset += sizeof(S0Config);
    }

    // move S0Config into plug config
    strcpy(plugs[0].name, rj45cfg_0.name);
    strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].name, s0config_0[0].name);
    strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].id,   s0config_0[0].id);
    if (strlen(s0config_0[0].id) > 0) plugs[0].activeDevices++;
    strcpy(plugs[0].devices[DEVICE_IDX_BROWN].name,  s0config_0[1].name);
    strcpy(plugs[0].devices[DEVICE_IDX_BROWN].id,    s0config_0[1].id);
    if (strlen(s0config_0[1].id) > 0) plugs[0].activeDevices++;
    strcpy(plugs[0].devices[DEVICE_IDX_GREEN].name,  s0config_0[2].name);
    strcpy(plugs[0].devices[DEVICE_IDX_GREEN].id,    s0config_0[2].id);
    if (strlen(s0config_0[2].id) > 0) plugs[0].activeDevices++;
    strcpy(plugs[0].devices[DEVICE_IDX_BLUE].name,   s0config_0[3].name);
    strcpy(plugs[0].devices[DEVICE_IDX_BLUE].id,     s0config_0[3].id);
    if (strlen(s0config_0[3].id) > 0) plugs[0].activeDevices++;

    strcpy(plugs[1].name, rj45cfg_1.name);
    plugs[1].activeDevices = 0;
    strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].name, s0config_1[0].name);
    strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].id,   s0config_1[0].id);
    if (strlen(s0config_1[0].id) > 0) plugs[1].activeDevices++;
    strcpy(plugs[1].devices[DEVICE_IDX_BROWN].name,  s0config_1[1].name);
    strcpy(plugs[1].devices[DEVICE_IDX_BROWN].id,    s0config_1[1].id);
    if (strlen(s0config_1[1].id) > 0) plugs[1].activeDevices++;
    strcpy(plugs[1].devices[DEVICE_IDX_GREEN].name,  s0config_1[2].name);
    strcpy(plugs[1].devices[DEVICE_IDX_GREEN].id,    s0config_1[2].id);
    if (strlen(s0config_1[2].id) > 0) plugs[1].activeDevices++;
    strcpy(plugs[1].devices[DEVICE_IDX_BLUE].name,   s0config_1[3].name);
    strcpy(plugs[1].devices[DEVICE_IDX_BLUE].id,     s0config_1[3].id);
    if (strlen(s0config_1[3].id) > 0) plugs[1].activeDevices++;

    // return
    return 0;
}

bool hasValidEndpointConfig() {
    return strlen(deviceCfg.endpoint) > 0 && strlen(deviceCfg.jwt) > 0;
}

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
    for (int i = 0; i < plug.activeDevices; i++) {
        S0Device device = plug.devices[i];
        sprintf(buffer, device.name, device.count);
        display.setCursor(0, y);
        display.println(buffer);
        y += 10;
    }
    display.display();
}

void writeDisplay(char *buffer) {
    display.setCursor(0, 10);
    display.clearDisplay();
    display.println(buffer);
    display.display();
}

void initDisplay() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    updateDisplay();
}

void initS0Pins() {
    for (int i = 0; i < RJ45_PLUG_COUNT; i++) {
        // get next plug
        RJ45 plug = plugs[i];

        // loop devices on plug
        for (int k = 0; k < plug.activeDevices; k++) {
            // get next device
            S0Device device = plug.devices[k];

            // init input pins and attach interrupt rutines
            pinMode(device.pinIRQ, INPUT);
            attachInterrupt(digitalPinToInterrupt(device.pinIRQ), device.isr, RISING);

            // ensure LED is not on
            pinMode(device.pinLED, OUTPUT);
            digitalWrite(device.pinLED, HIGH);
            delay(200);
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

int httpPostData(char *data) {
    Log.trace(F("Starting POST of data to server" CR));

    // prepare headers
    char bufferAuthHeader[400];
    sprintf(bufferAuthHeader, "Bearer %s", deviceCfg.jwt);
    Log.trace(F("Constructed value for Authorization header" CR));

    WiFiClientSecure *client = new WiFiClientSecure;
    if (client) {
        Log.trace(F("Constructed wifi secure client" CR));
        client->setCACert(rootCACertificate);
        Log.trace(F("Set root CA certificate" CR));
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            if (https.begin(*client, deviceCfg.endpoint)) {
                // start connection and send HTTP headers
                Log.trace(F("Opened connection to endpoint: %s" CR), deviceCfg.endpoint);
                https.addHeader("Authorization", bufferAuthHeader);
                https.addHeader("Content-Type", "application/json");
                https.addHeader("Accept", "application/json");

                // do post
                int httpCode = https.POST(data);

                // httpCode will be negative on error
                if (httpCode > 0) {
                    // HTTP header has been send and Server response header has been handled
                    Log.trace(F("Did POST to server - httpCode: %d" CR), httpCode);

                    // file found at server
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                        String payload = https.getString();
                    }
                } else {
                    Log.warning(F("POST failed, error: %s"), https.errorToString(httpCode).c_str());
                }

                https.end();
            } else {
                Log.warning(F("Unable to connect to endpoint: %s"), deviceCfg.endpoint);
            }
        }

        delete client;
        return 0;

    } else {
        Log.fatal(F("Unable to create wifi secure client"));
        return 1;
    }
}

/**
 * Get Mac address to use.
 */
void getMacAddress(byte *mac) {
    WiFi.macAddress(mac);
}

/**
 * Convert mac address to a char buffer.
 */
void getMacAddressString(char *buffer) {
    byte mac[6];
    getMacAddress(mac);
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * Convert mac address to a char buffer.
 */
void getMacAddressStringNoColon(char *buffer) {
    byte mac[6];
    getMacAddress(mac);
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * Print MAC address to serial console
 */
void printMacAddress() {
    // print MAC address
    char buf[20];
    getMacAddressString(buf);
    Log.notice("MAC address: %s", buf);
}

void printIPAddress() {
    Log.notice("Received IP: %s", WiFi.localIP());
}

void buildNetworkName(char* buffer) {
  char mac_addr[18];
  getMacAddressStringNoColon(mac_addr);
  sprintf(buffer, "S0Monitor-%s", mac_addr);
}

void prepareDataPayload(char *jsonBuffer, size_t size, RJ45 *workPlugs, unsigned long sampleDuration) {
    // create document
    StaticJsonDocument<2048> doc;

    // get your MAC address
    char mac_addr[20];
    getMacAddressString(mac_addr);

    // build payload
    doc["msgtype"] = "data";
    doc["deviceId"].set(mac_addr);
    JsonArray jsonData = doc.createNestedArray("data");

    // loop sensors and add data
    for (uint8_t i = 0; i < RJ45_PLUG_COUNT; i++) {
        RJ45 plug = workPlugs[i];
        for (uint8_t k = 0; k < plug.activeDevices; k++) {
            S0Device device = plug.devices[k];
            JsonObject jsonSensorData = jsonData.createNestedObject();
            jsonSensorData["sensorId"] = device.id;
            jsonSensorData["sensorValue"].set(device.count);
            jsonSensorData["sensorDuration"] = sampleDuration;
        }
    }

    // serialize
    serializeJson(doc, jsonBuffer, size);
}

void prepareControlRestartPayload(char *jsonBuffer, size_t size) {
    // create document
    StaticJsonDocument<256> doc;

    // get your MAC and IP address
    char mac_addr[20];
    getMacAddressString(mac_addr);

    // build payload
    doc["msgtype"] = "control";
    doc["deviceId"].set(mac_addr);
    JsonObject jsonData = doc.createNestedObject("data");
    jsonData["restart"] = true;
    jsonData["ip"].set(WiFi.localIP().toString());

    // serialize
    serializeJson(doc, jsonBuffer, size);
}

/**
 * Sends a restart control message to the server at the configured endpoint 
 * if we have endpoint config and JWT, the the device just came up and 
 * we are connected to wifi.
 */
void pingServerOnStart() {
    if (justReset && hasValidEndpointConfig() && WiFi.isConnected()) {
        // this is the first run - tell web server we restarted
        Log.trace(F("This is first run through loop() since starting - tell web server we restarted" CR));
        justReset = false;

        // prep payload
        char payload[256];
        prepareControlRestartPayload(payload, sizeof(payload));
        Log.trace(F("Generated payload for web server: %s" CR), payload);

        // send payload
        httpPostData(payload);
    }
}

void initializeWifi() {
    // init wifi
    Log.trace(F("Starting to initiate wi-fi connection" CR));
    WiFi.begin(wifiCfg.ssid, wifiCfg.password);
    int wifiDelayCount = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        ++wifiDelayCount;
        char buffer[64];
        sprintf(buffer, "Initializing wifi (%d)" CR, wifiDelayCount);
        Log.trace(buffer);
        writeDisplay(buffer);
    }
    printIPAddress();
    printMacAddress();
}

/**
 * Using configuration from the configured RJ45 plugs will initialize 
 * the pins as required. Whether in use or not we will still configure 
 * them.
 */
void initializePins() {
    // setup plugs and connections per plug
    Log.trace(F("Initializing pins" CR));
    initISR();
    initS0Pins();
}

void setup() {
    // setup serial and init display
    Serial.begin(115200);

    // Initialize with log level and log output. 
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        for (;;) {
            Log.fatal(F("SSD1306 display allocation failed"));
            delay(5000);
        }
    }
    initDisplay();

    // start AP
    char ssid[32];
    buildNetworkName(ssid);
    WiFi.softAP(ssid, "");
    server = new ConfigWebServer(&deviceCfg, &wifiCfg);
    Log.trace(F("Started AP with SSID: %s" CR), ssid);

    // read config
    int rc = readConfiguration();
    if (!rc) {
        // we couldn't read a valid config

    } else {
        // we have valid config
        initializeWifi();
        initializePins();
        samplePeriodStart = millis();
    }
}

void loop() {
    // disable AP 
    if ((WiFi.softAPIP() && millis() > DELAY_TURNOFF_AP) && wifiCfg.keep_ap_on == false) {
        // diable AP
        Log.trace(F("Disabling AP..." CR));
        WiFi.softAPdisconnect(false);
        WiFi.enableAP(false);
    }

    // send control message to server if we just came up
    pingServerOnStart();

    // update current time
    now = millis();
    boolean shouldUpdateDisplay = false;

    // loop plugs and turn off leds that might be lit up
    for (int i = 0; i < RJ45_PLUG_COUNT; i++) {
        // get next plug
        RJ45 plug = plugs[i];

        // loop devices on plug
        for (int k = 0; k < plug.activeDevices; k++) {
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
        if (plugs[1].activeDevices == 0 || page >= RJ45_PLUG_COUNT) page = 0;
        lastPageChange = now;
    }

    // update display if appropriate
    if (shouldUpdateDisplay) updateDisplay();

    if (WiFi.isConnected() && now - samplePeriodStart > SAMPLE_DURATION) {
        Log.trace(F("Sample period lapsed - will post to server..." CR));

        // copy S0Device structs
        RJ45 plugs_copy[RJ45_PLUG_COUNT];
        for (uint8_t i = 0; i < RJ45_PLUG_COUNT; i++) {
            plugs_copy[i].activeDevices = plugs[i].activeDevices;
            for (uint8_t j = 0; j < plugs[i].activeDevices; j++) {
                plugs_copy[i].devices[j] = plugs[i].devices[j];
                plugs[i].devices[j].count = 0;
                plugs_copy[i].devices[j].isr = 0;
            }
        }

        // keep track of when we last sent data
        samplePeriodStart = now;

        // prepare payload
        char payload[2048];
        prepareDataPayload(payload, sizeof(payload), plugs_copy, SAMPLE_DURATION);
        Log.trace(F("Prepared data payload for server: %s" CR), payload);
        httpPostData(payload);
    }
}
