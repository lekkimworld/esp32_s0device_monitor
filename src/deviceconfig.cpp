#include "version.h"
#include "logging.h"
#include "deviceconfig.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

extern WifiConfig wificonfig;
extern DeviceConfig deviceconfig;
extern RJ45Config rj45config[];
extern S0Config s0config[];
extern RJ45 plugs_runtime[];
extern int httpCode;
extern String httpData; 

String _templateProcessor(const String& var) {
    if (var == "TITLE") return F("S0 Monitor");
    if (var == "VERSION_NUMBER") return VERSION_NUMBER;
    if (var == "VERSION_LASTCHANGE") return VERSION_LASTCHANGE;
    if (var == "HTTP_CODE") {
        char buffer[4];
        sprintf(buffer, "%d", httpCode);
        return String(buffer);
    };
    if (var == "HTTP_DATA") return httpData;
    return String();
}

ConfigWebServer::ConfigWebServer() {
    S0_LOG_DEBUG("[DEVICECONFIG] Constructing ConfigWebServer instance");
}

void ConfigWebServer::_wifiConfig() {
    this->server->on("/wificonfig.html", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/wificonfig.html", String(), false, _templateProcessor);
    });
    this->server->on("/wificonfig.json", HTTP_GET, [this](AsyncWebServerRequest *request){
        char passbuf[10];
        sprintf(passbuf, "%.4s****", wificonfig.password);

        StaticJsonDocument<256> doc;
        doc["ssid"].set(wificonfig.ssid);
        doc["password"].set(passbuf);
        doc["keep_ap_on"].set(wificonfig.keep_ap_on);;
        doc["status"].set(WiFi.status() == WL_CONNECTED ? "Connected" : "NOT connected");

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        
        request->send(response);
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/wificonfig.save", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        // get data
        JsonObject jsonObj = json.as<JsonObject>();
        const char *ssid = jsonObj["ssid"];
        const char *pass = jsonObj["password"];
        const bool keep_on = jsonObj["keep_ap_on"].as<bool>();
        S0_LOG_DEBUG("[DEVICECONFIG] Read POSTed parameters:");
        S0_LOG_DEBUG("[DEVICECONFIG]  > ssid: <%s>", ssid);
        S0_LOG_DEBUG("[DEVICECONFIG]  > password: <%s>", pass);
        S0_LOG_DEBUG("[DEVICECONFIG]  > keep_ap_on: <%d>", keep_on);

        // abort if no callback
        bool didSave = false;
        int rc = 0;
        if (this->wifiFunc == 0) {
            S0_LOG_ERROR("[DEVICECONFIG] No wifi callback set - nothing will be saved");
        } else {
            // create new struct and call callback
            S0_LOG_INFO("[DEVICECONFIG] Wifi callback set - prepping data for callback");
            WifiConfig cfg;
            strcpy(cfg.ssid, ssid);
            strcpy(cfg.password, pass);
            cfg.keep_ap_on = keep_on;

            // perform callback
            S0_LOG_DEBUG("[DEVICECONFIG] Calling wifi callback");
            rc = this->wifiFunc(&cfg);
            didSave = true;
            S0_LOG_DEBUG("[DEVICECONFIG] Returned from wifi callback");
        }
        
        // send response
        S0_LOG_DEBUG("[DEVICECONFIG] Sending response to caller");
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        response->setCode(200);
        if (didSave) {
            response->println("{\"status\": \"ok\"}");
        } else {
            response->println("{\"status\": \"error\", \"message\": \"no callback present\"}");
        }
        request->send(response);
        S0_LOG_DEBUG("[DEVICECONFIG] Sent response to caller");

        // restart if callback returned 0
        if (!rc) {
            ESP.restart();
            // restart() doesn't always end execution
            while (true) {
                yield();
            }
        }
    });
    this->server->addHandler(handler);
}

void ConfigWebServer::_deviceConfig() {
    this->server->on("/deviceconfig.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/deviceconfig.html", String(), false, _templateProcessor);
    });
    this->server->on("/deviceconfig.json", HTTP_GET, [this](AsyncWebServerRequest *request){
        char jwtbuf[10];
        sprintf(jwtbuf, "%.10s****", deviceconfig.jwt);

        StaticJsonDocument<256> doc;
        doc["delay_post"].set(deviceconfig.delay_post);
        doc["endpoint"].set(deviceconfig.endpoint);
        doc["jwt"].set(jwtbuf);
        doc["prod_cert"].set(deviceconfig.productionCert);
        doc["use_display"].set(deviceconfig.useDisplay);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        
        request->send(response);
    });
    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/deviceconfig.save", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        // get data
        JsonObject jsonObj = json.as<JsonObject>();
        const int delay_post = jsonObj["delay_post"].as<int>();
        const char *endpoint = jsonObj["endpoint"];
        const char *jwt = jsonObj["jwt"];
        const bool prod_cert = jsonObj["prod_cert"].as<bool>();
        const bool use_display = jsonObj["use_display"].as<bool>();
        S0_LOG_DEBUG("[DEVICECONFIG] Read POSTed parameters:");
        S0_LOG_DEBUG("[DEVICECONFIG]  > delay_post: <%u>", delay_post);
        S0_LOG_DEBUG("[DEVICECONFIG]  > endpoint: <%s>", endpoint);
        S0_LOG_DEBUG("[DEVICECONFIG]  > jwt: <%s>", jwt);
        S0_LOG_DEBUG("[DEVICECONFIG]  > prod_cert: <%d>", prod_cert);
        S0_LOG_DEBUG("[DEVICECONFIG]  > use_display: <%d>", use_display);

        // abort if no callback
        bool didSave = false;
        int rc = 0;
        if (this->deviceFunc == 0) {
            S0_LOG_ERROR("[DEVICECONFIG] No device callback set - nothing will be saved");
        } else {
            // create new struct and call callback
            S0_LOG_INFO("[DEVICECONFIG] Device callback set - prepping data for callback");
            DeviceConfig cfg;
            cfg.delay_post = delay_post;
            strcpy(cfg.endpoint, endpoint);
            strcpy(cfg.jwt, jwt);
            cfg.productionCert = prod_cert;
            cfg.useDisplay = use_display;

            // perform callback
            S0_LOG_DEBUG("[DEVICECONFIG] Calling device callback");
            rc = this->deviceFunc(&cfg);
            didSave = true;
            S0_LOG_DEBUG("[DEVICECONFIG] Returned from device callback");
        }
        
        // send response
        S0_LOG_DEBUG("[DEVICECONFIG] Sending response to caller");
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        response->setCode(200);
        if (didSave) {
            response->println("{\"status\": \"ok\"}");
        } else {
            response->println("{\"status\": \"error\", \"message\": \"no callback present\"}");
        }
        request->send(response);
        S0_LOG_DEBUG("[DEVICECONFIG] Sent response to caller");

        // restart if callback returned 0
        if (!rc) {
            ESP.restart();
            // restart() doesn't always end execution
            while (true) {
                yield();
            }
        }
    });
    this->server->addHandler(handler);
}

void ConfigWebServer::_plugConfig() {
    this->server->on("/plugconfig.html", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/plugconfig.html", String(), false, _templateProcessor);
    });
    this->server->on("/plugconfig.json", HTTP_GET, [this](AsyncWebServerRequest *request){
        StaticJsonDocument<768> doc;
        char buffer[20];
        for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
            sprintf(buffer, "plug_%d_name", i);
            doc[buffer].set(rj45config[i].name);

            for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
                uint8_t idx = i * DEVICES_PER_PLUG + k;
                sprintf(buffer, "plug_%d_device_%d_id", i, k);
                doc[buffer].set(s0config[idx].id);
                sprintf(buffer, "plug_%d_device_%d_name", i, k);
                doc[buffer].set(s0config[idx].name);
            }
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/plugconfig.save", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        // get data
        JsonObject jsonObj = json.as<JsonObject>();
        S0_LOG_DEBUG("[DEVICECONFIG] Read POSTed parameters:");
        char buffer[20];
        for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
            sprintf(buffer, "plug_%d_name", i);
            S0_LOG_DEBUG("[DEVICECONFIG]  > %s: <%s>", buffer, jsonObj[buffer].as<char*>());

            for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
                sprintf(buffer, "plug_%d_device_%d_id", i, k);
                S0_LOG_DEBUG("[DEVICECONFIG]  > %s: <%s>", buffer, jsonObj[buffer].as<char*>());
                sprintf(buffer, "plug_%d_device_%d_name", i, k);
                S0_LOG_DEBUG("[DEVICECONFIG]  > %s: <%s>", buffer, jsonObj[buffer].as<char*>());
            }
        }

        // abort if no callback
        bool didSave = false;
        int rc = 0;
        if (this->plugFunc == 0) {
            S0_LOG_ERROR("[DEVICECONFIG] No plug callback set - nothing will be saved");
        } else {
            // create new struct and call callback
            S0_LOG_INFO("[DEVICECONFIG] Plug callback set - prepping data for callback");
            RJ45Config rj[RJ45_PLUG_COUNT];
            S0Config s0[DEVICES_PER_PLUG * RJ45_PLUG_COUNT];
            for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
                sprintf(buffer, "plug_%d_name", i);
                strcpy(rj[i].name, jsonObj[buffer].as<char*>());

                for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
                    uint8_t idx = i * DEVICES_PER_PLUG + k;
                    sprintf(buffer, "plug_%d_device_%d_id", i, k);
                    strcpy(s0[idx].id, jsonObj[buffer].as<char*>());
                    sprintf(buffer, "plug_%d_device_%d_name", i, k);
                    strcpy(s0[idx].name, jsonObj[buffer].as<char*>());
                }
            }

            // perform callback
            S0_LOG_DEBUG("[DEVICECONFIG] Calling plug callback");
            rc = this->plugFunc(rj, s0);
            didSave = true;
            S0_LOG_DEBUG("[DEVICECONFIG] Returned from plug callback");
        }
        
        // send response
        S0_LOG_DEBUG("[DEVICECONFIG] Sending response to caller");
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        response->setCode(200);
        if (didSave) {
            response->println("{\"status\": \"ok\"}");
        } else {
            response->println("{\"status\": \"error\", \"message\": \"no callback present\"}");
        }
        request->send(response);
        S0_LOG_DEBUG("[DEVICECONFIG] Sent response to caller");

        // restart if callback returned 0
        if (!rc) {
            ESP.restart();
            // restart() doesn't always end execution
            while (true) {
                yield();
            }
        }
    });
    this->server->addHandler(handler);
}

void ConfigWebServer::_sensorData() {
    this->server->on("/sensordata.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/sensordata.html", String(), false, _templateProcessor);
    });
    this->server->on("/sensordata.json", HTTP_GET, [this](AsyncWebServerRequest *request){
        StaticJsonDocument<1024> doc;
        char buffer[20];
        for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
            for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
                uint8_t idx = i * DEVICES_PER_PLUG + k;
                sprintf(buffer, "plug_%d_device_%d_name", i, k);
                doc[buffer].set(s0config[idx].name);
                sprintf(buffer, "plug_%d_device_%d_value", i, k);
                doc[buffer].set(plugs_runtime[i].devices[k].count);
            }
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    });
}

void ConfigWebServer::setWifiChangedCallback(WifiConfigChangedCallback wifiFunc) {
    this->wifiFunc = wifiFunc;
}

void ConfigWebServer::setDeviceChangedCallback(DeviceConfigChangedCallback deviceFunc) {
    this->deviceFunc = deviceFunc;
}

void ConfigWebServer::setPlugChangedCallback(PlugConfigChangedCallback plugFunc) {
    this->plugFunc = plugFunc;
}

void ConfigWebServer::init() {
    if (!SPIFFS.begin(true)) {
        S0_LOG_ERROR("[DEVICECONFIG] Unable to initialize SPIFFS");
        return;
    }

    // create server
    this->server = new AsyncWebServer(80);
    
    // ******************** wifi config
    this->_wifiConfig();

    // ******************** plug config
    this->_plugConfig();

    // ******************** device config
    this->_deviceConfig();

    // ******************* sensor data
    this->_sensorData();

    // add static routes
    this->server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });
    this->server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/root.html", String(), false, _templateProcessor);
    });
    this->server->on("/httpstatus.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/httpstatus.html", String(), false, _templateProcessor);
    });
    this->server->on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/styles.css");
    });
    this->server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/script.js");
    });

    this->server->on("/version.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        char buffer[1024];
        sprintf(buffer, "{\"version\": \"%s\", \"change\": \"%s\"}", VERSION_NUMBER, VERSION_LASTCHANGE);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        response->setCode(200);
        response->print(buffer);
        request->send(response);
    });

    S0_LOG_INFO("[DEVICECONFIG] Constructed ConfigWebServer instance - starting to accept incoming requests");
    this->server->begin();
}

ConfigWebServer::~ConfigWebServer() {
    S0_LOG_INFO("[DEVICECONFIG] Destructing ConfigWebServer instance - ending server and releasing memory");
    this->server->end();
    delete this->server;
    SPIFFS.end();
}
