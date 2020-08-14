#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <Syslog.h>
#include "isr.h"
#include "credentials.h"

// DST Root CA X3, https://letsencrypt.org/certs/trustid-x3-root.pem.txt
const char* rootCACertificate_DSTRootCAX3 = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
"-----END CERTIFICATE-----\n";

// Let's Encrypt, SRG Root X1 (self-signed), https://letsencrypt.org/certs/isrgrootx1.pem.txt
const char* rootCACertificate_LetsEncrypt = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

// DigiCert High Assurance EV Root CA, https://www.digicert.com/digicert-root-certificates.htm
const char* rootCACertificate_Digicert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
"-----END CERTIFICATE-----\n";
const char* rootCACertificate = rootCACertificate_Digicert;

// declaration
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long now = millis();
unsigned long lastPageChange = 0;
int page = 0;
unsigned long SAMPLE_DURATION = 2 * 60 * 1000;
unsigned long samplePeriodStart = 0;
bool justReset = true;
bool useDisplay = false;

// Create a new syslog instance with LOG_KERN facility
WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
bool hasSyslog();

template<typename... Args>
void S0_LOG_DEBUG(const char* msg, Args... args) {
  Serial.print("[DEBUG] - ");
  Serial.printf(msg, args...);
  Serial.println();
  if (hasSyslog()) syslog.logf(LOG_KERN, msg, args...); 
}

template<typename... Args>
void S0_LOG_INFO(const char* msg, Args... args) {
  Serial.print(" [INFO] - ");
  Serial.printf(msg, args...);
  Serial.println();
  if (hasSyslog()) syslog.logf(LOG_INFO, msg, args...); 
}

template<typename... Args>
void S0_LOG_ERROR(const char* msg, Args... args) {
  Serial.print("[ERROR] - ");
  Serial.printf(msg, args...);
  Serial.println();
  if (hasSyslog())  syslog.logf(LOG_ERR, msg, args...); 
}

bool hasSyslog() {
  return strlen(SYSLOG_SERVER) > 0;
}

bool hasWebEndpoint() {
  return strlen(MY_ENDPOINT_URL) > 0;
}

void updateDisplay() {
  if (!useDisplay) return; 
  
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
  for (int i=0; i<plug.activeDevices; i++) {
    S0Device device = plug.devices[i];
    sprintf(buffer, device.name, device.count);
    display.setCursor(0, y);
    display.println(buffer);
    y += 10;
  }
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

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    S0_LOG_ERROR("SSD1306 display allocation failed");
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  updateDisplay();
}

void initS0Pins() {
  for (int i=0; i<RJ45_PLUG_COUNT; i++) {
    // get next plug
    RJ45 plug = plugs[i];
    
    // loop devices on plug
    for (int k=0; k<plug.activeDevices; k++) {
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
  // prepare headers
  uint16_t contentLength = strlen(data) + 4;
  char str_contentLength[5];
  sprintf (str_contentLength, "%4i", contentLength);
  char bufferAuthHeader[400];
  sprintf(bufferAuthHeader, "Bearer %s", MY_DEVICE_JWT);
  S0_LOG_DEBUG("Free heap: %d", ESP.getFreeHeap());

  WiFiClientSecure *client = new WiFiClientSecure;
  if (client) {
    client -> setCACert(rootCACertificate);
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
 * Print MAC address to serial console
 */
void printMacAddress() {
  // print MAC address
  char buf[20];
  getMacAddressString(buf);
  S0_LOG_INFO("MAC address: %s", buf);
}

void printIPAddress() {
  S0_LOG_INFO("Received IP: %s", WiFi.localIP().toString().c_str());
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
  for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
    RJ45 plug = workPlugs[i];
    for (uint8_t k=0; k<plug.activeDevices; k++) {
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

void setup() {
  // setup serial and init display
  Serial.begin(115200);
  
  // init the display
  initDisplay();

  // init wifi
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASS);
  int wifiDelayCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    ++wifiDelayCount;
    char buffer[64];
    sprintf(buffer, "Initializing wifi (%d)", wifiDelayCount);
    writeDisplay(buffer);
    S0_LOG_INFO(buffer);
  }
  printIPAddress();
  printMacAddress();

  // setup plugs and connections per plug
  strcpy(plugs[0].name, "Yellow RJ45");
  strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].name, "Tumbler (Or)   : %d");
  strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].id,   "s0dryer");
  strcpy(plugs[0].devices[DEVICE_IDX_BROWN].name,  "Vaskemask. (Br): %d");
  strcpy(plugs[0].devices[DEVICE_IDX_BROWN].id,    "s0washer");
  strcpy(plugs[0].devices[DEVICE_IDX_GREEN].name,  "Opvask. (Gr)   : %d");
  strcpy(plugs[0].devices[DEVICE_IDX_GREEN].id,    "s0dishwasher");
  strcpy(plugs[0].devices[DEVICE_IDX_BLUE].name,   "Varmepumpe (Bl): %d");
  strcpy(plugs[0].devices[DEVICE_IDX_BLUE].id,     "s0heatpump");
  plugs[0].activeDevices = 4;

  strcpy(plugs[1].name, "White RJ45");
  strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].name, "Gulvvame, bad  : %d");
  strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].id,   "s0floorheat");
  plugs[1].activeDevices = 1;

  S0_LOG_INFO("Initializing pins");
  initISR();
  initS0Pins();
  samplePeriodStart = millis();
}

void loop() {
  // send control message to server if we just came up
  pingServerOnStart();

  // update current time
  now = millis();
  boolean shouldUpdateDisplay = false;
  
  // loop plugs and turn off leds that might be lit up
  for (int i=0; i<RJ45_PLUG_COUNT; i++) {
    // get next plug
    RJ45 plug = plugs[i];

    // loop devices on plug
    for (int k=0; k<plug.activeDevices; k++) {
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
    if (plugs[1].activeDevices ==0 || page >= RJ45_PLUG_COUNT) page = 0;
    lastPageChange = now;
  }

  // update display if appropriate
  if (shouldUpdateDisplay) updateDisplay();

  if (now - samplePeriodStart > SAMPLE_DURATION) {
    S0_LOG_INFO("Will post to server...");
    
    // keep track of when we last sent data
    samplePeriodStart = now;
    S0_LOG_DEBUG("Set timestamp samplePeriodStart to now");
    
    // copy S0Device structs
    RJ45 plugs_copy[RJ45_PLUG_COUNT];
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
      plugs_copy[i].activeDevices = plugs[i].activeDevices;
      for (uint8_t j=0; j<plugs[i].activeDevices; j++) {
        plugs_copy[i].devices[j] = plugs[i].devices[j];
        plugs[i].devices[j].count = 0;
        plugs_copy[i].devices[j].isr = 0;
      }
    }
    S0_LOG_DEBUG("Made memory copy of data structs");

    // prepare payload
    char payload[2048];
    prepareDataPayload(payload, sizeof(payload), plugs_copy, SAMPLE_DURATION);
    S0_LOG_DEBUG("Prepared payload");

    httpPostData(payload);
    S0_LOG_DEBUG("Done attempting to send payload");
  }
}
