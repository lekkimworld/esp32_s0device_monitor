#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "constants.h"
#include "types.h"
#include "declarations.h"
#include "config.h"
#include "pem.h"
#include "utils.h"
#include "credentials.h"
#include "deviceconfig.h"
#include "isr.h"
#include "logging.h"
#include "display.h"

void initializePins() {
    // init pin numbers etc.
    initializeISR();

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
            S0_LOG_DEBUG("Blinking LED: %d", device.pinLED);
            pinMode(device.pinLED, OUTPUT);
            digitalWrite(device.pinLED, HIGH);
            delay(200);
            digitalWrite(device.pinLED, LOW);
        }
    }
}

void initializeWifi() {
    // init wifi
    S0_LOG_INFO("Starting to initiate wi-fi connection");
    WiFi.begin(wifiCfg.ssid, wifiCfg.password);
    int wifiDelayCount = 0;
    while (WiFi.status() != WL_CONNECTED && wifiDelayCount < 10) {
        delay(1000);
        ++wifiDelayCount;
        char buffer[64];
        sprintf(buffer, "Initializing wifi (%d)", wifiDelayCount);
        S0_LOG_DEBUG(buffer);
        writeDisplay(buffer);
    }

    S0_LOG_INFO("Received IP: %s", WiFi.localIP().toString().c_str());
    
    char mac_addr[32];
    getMacAddressString(mac_addr);
    S0_LOG_INFO("MAC address: %s", mac_addr);
}

void turnOffLed(S0Device *device) {
    if (device->startTimer && (now - device->lastTrigger > LED_BLINK_TIME)) {
        digitalWrite(device->pinLED, LOW);
        device->startTimer = false;
    }
}

int httpPostData(char *data) {
    // prepare headers
    uint16_t contentLength = strlen(data) + 4;
    char str_contentLength[5];
    sprintf(str_contentLength, "%4i", contentLength);
    char bufferAuthHeader[400];
    sprintf(bufferAuthHeader, "Bearer %s", MY_DEVICE_JWT);
    S0_LOG_DEBUG("Free heap: %d", ESP.getFreeHeap());

    WiFiClientSecure *client = new WiFiClientSecure;
    if (client) {
        const char * rootCACertificate = deviceCfg.productionCert 
            ? rootCACertificate_DSTRootCAX3_HerokuCustomDoamin 
            : rootCACertificate_Digicert_HerokuDefaultDomain;

        client->setCACert(rootCACertificate);
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            S0_LOG_DEBUG("Free heap: %d", ESP.getFreeHeap());

            S0_LOG_DEBUG("[HTTPS] begin...");
            if (https.begin(*client, MY_ENDPOINT_URL)) {
                // start connection and send HTTP headers
                S0_LOG_DEBUG("[HTTPS] Sending headers...");
                https.addHeader("Authorization", bufferAuthHeader);
                https.addHeader("Content-Type", "application/json");
                https.addHeader("Accept", "application/json");
                https.addHeader("Content-Length", str_contentLength);

                // do post
                S0_LOG_DEBUG("[HTTPS] POSTing data");
                int httpCode = https.POST(data);
                S0_LOG_DEBUG("[HTTPS] Called https.POST - httpCode: %d", httpCode);
                S0_LOG_DEBUG("Free heap: %d", ESP.getFreeHeap());

                // httpCode will be negative on error
                if (httpCode > 0) {
                    // HTTP header has been send and Server response header has been handled
                    S0_LOG_DEBUG("[HTTPS] POST... code: %d", httpCode);

                    // file found at server
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                        String payload = https.getString();
                        S0_LOG_DEBUG("[HTTPS] Received response: %s", payload.c_str());
                    }
                } else {
                    S0_LOG_ERROR("[HTTPS] https.POST failed, code: %d, error: %s", httpCode, https.errorToString(httpCode).c_str());
                }

                https.end();
            } else {
                S0_LOG_ERROR("[HTTPS] Unable to connect\n");
            }

            // End extra scoping block
        }
        S0_LOG_DEBUG("Free heap: %d", ESP.getFreeHeap());

        delete client;
        return 1;

    } else {
        S0_LOG_ERROR("Unable to create client");
        return 0;
    }
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
 * Sends a restart control message to the server at the configured endpoint if there is 
 * an endpoint and the device just came up.
 */
void pingServerOnStart() {
    if (justReset && hasWebEndpoint()) {
        // this is the first run - tell web server we restarted
        justReset = false;

        // prep payload
        char payload[256];
        prepareControlRestartPayload(payload, sizeof(payload));

        // send payload
        httpPostData(payload);
    }
}


// ********** MAIN METHODS **************

void setup() {
    // setup serial and init display
    Serial.begin(115200);

    // init the display
    initDisplay();

    // start AP
    char ssid[32];
    buildNetworkName(ssid);
    WiFi.softAP(ssid, "");
    softAPEnabled = true;
    server = new ConfigWebServer(&deviceCfg, &wifiCfg);
    server->setWifiChangedCallback([](WifiConfig *wifiCfg) {
        S0_LOG_DEBUG("Received callback from ConfigWebServer - something changed WifiConfig");
        writeConfiguration(wifiCfg);
        delay(500);
        ESP.restart();
    });
    server->setDeviceChangedCallback([](DeviceConfig *deviceCfg) {
        S0_LOG_DEBUG("Received callback from ConfigWebServer - something changed DeviceConfig");
    });
    server->init();
    S0_LOG_DEBUG("Started AP with SSID: %s", ssid);

    // read config
    int rc = readConfiguration();
    if (rc) {
        // we couldn't read a valid config
        S0_LOG_ERROR("Couldn't read valid config - will not start device");
        
    } else {
        // we have valid config
        S0_LOG_INFO("Initializing ISR's and pins");
        initializeWifi();
        initializePins();
        samplePeriodStart = millis();
    }
}

void loop() {
    // disable AP
    if (softAPEnabled && (millis() > DELAY_TURNOFF_AP) && wifiCfg.keep_ap_on == false) {
        // diable AP
        S0_LOG_DEBUG("Disabling AP...");
        WiFi.softAPdisconnect(false);
        WiFi.enableAP(false);
        softAPEnabled = false;
    }

    if (WiFi.status() == WL_CONNECTED && hasWebEndpoint()) {
        // connected to wifi - send control message to server if we just came up
        pingServerOnStart();
    }

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

        // go to page 0 if over number of plugs
        if (page > RJ45_PLUG_COUNT) page = 0;
        lastPageChange = now;
    }

    // update display if appropriate
    if (shouldUpdateDisplay) {
        updateDisplay();
        page++;
    }

    if (WiFi.status() == WL_CONNECTED && hasWebEndpoint() && now - samplePeriodStart > deviceCfg.delay_post) {
        S0_LOG_INFO("Will post to server...");

        // keep track of when we last sent data
        samplePeriodStart = now;
        S0_LOG_DEBUG("Set timestamp samplePeriodStart to now");

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
        S0_LOG_DEBUG("Made memory copy of data structs");

        // prepare payload
        char payload[2048];
        prepareDataPayload(payload, sizeof(payload), plugs_copy, deviceCfg.delay_post);
        S0_LOG_DEBUG("Prepared payload");

        httpPostData(payload);
        S0_LOG_DEBUG("Done attempting to send payload");
    }
}
