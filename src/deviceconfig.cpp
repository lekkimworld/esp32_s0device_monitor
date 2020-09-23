#include "version.h"
#include "logging.h"
#include "deviceconfig.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

DeviceConfig *_deviceCfg;

void _header(AsyncResponseStream *response, bool back, const char* title) {
    response->println("<!DOCTYPE html><html><head>" \
        "<meta name=\"viewport\" content=\"initial-scale=1.0\">" \
        "<title>S0Monitor</title>" \
        "<link rel=\"stylesheet\" href=\"/styles.css\">" \
        "</head>" \
        "<body>");
    if (back) response->println("<div class=\"position\"><a href=\"./\">Back</a></div>");
    response->printf("<div class=\"position title\">%s</div>", title);
}
void _footer(AsyncResponseStream *response) {
    response->printf("<div class=\"position footer right\">%s<br/>%s</div>", VERSION_NUMBER, VERSION_LASTCHANGE);
    response->println("</body></html>");
}

ConfigWebServer::ConfigWebServer(DeviceConfig *deviceCfg, WifiConfig *wifiCfg) {
    S0_LOG_DEBUG("Constructing ConfigWebServer instance");
    this->deviceCfg = deviceCfg;
    this->wifiCfg = wifiCfg;
}

void ConfigWebServer::setWifiChangedCallback(WifiConfigChangedCallback wifiFunc) {
    this->wifiFunc = wifiFunc;
}

void ConfigWebServer::setDeviceChangedCallback(DeviceConfigChangedCallback deviceFunc) {
    this->deviceFunc = deviceFunc;
}

void ConfigWebServer::init() {
    if (!SPIFFS.begin(true)) {
        S0_LOG_ERROR("Unable to initialize SPIFFS");
        return;
    }

    // create server
    this->server = new AsyncWebServer(80);

    // add routes
    this->server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });
    this->server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/root.html");
    });
    this->server->on("/wificonfig.html", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/wificonfig.html");
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/wifi.save", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        // get data
        JsonObject jsonObj = json.as<JsonObject>();
        const char *ssid = jsonObj["ssid"];
        const char *pass = jsonObj["password"];
        const bool keep_on = jsonObj["keep_ap_on"];
        S0_LOG_DEBUG("Read POSTed parameters:");
        S0_LOG_DEBUG(" > ssid: <%s>", ssid);
        S0_LOG_DEBUG(" > password: <%s>", pass);
        S0_LOG_DEBUG(" > keep_ap_on: <%d>", keep_on);

        // abort if no callback
        bool didSave = false;
        int rc = 0;
        if (this->wifiFunc == 0) {
            S0_LOG_ERROR("No wifi callback set - nothing will be saved");
        } else {
            // create new struct and call callback
            S0_LOG_INFO("Wifi callback set - prepping data for callback");
            WifiConfig cfg;
            strcpy(cfg.ssid, ssid);
            strcpy(cfg.password, pass);
            cfg.keep_ap_on = keep_on;

            // perform callback
            S0_LOG_DEBUG("Calling wifi callback");
            rc = this->wifiFunc(&cfg);
            didSave = true;
            S0_LOG_DEBUG("Returned from wifi callback");
        }
        
        // send response
        S0_LOG_DEBUG("Sending response to caller");
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        response->setCode(200);
        if (didSave) {
            response->println("{\"status\": \"ok\"}");
        } else {
            response->println("{\"status\": \"error\", \"message\": \"no callback present\"}");
        }
        request->send(response);

        // restart if callback returned 0
        if (!rc) ESP.restart();
    });
    this->server->addHandler(handler);

    this->server->on("/deviceconfig.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->setCode(200);

        _header(response, true, "Device Config.");
        response->print("<div class=\"position menuitem\">");
        response->print("<p>");
        response->print("</p>");
        response->print("<form method=\"post\" action=\"/device.save\">");
        response->print("<table border=\"0\">");
        response->print("<tr><td align=\"left\">Foo</td><td><input type=\"text\" name=\"ssid\" autocomplete=\"off\"></input></td></tr>");
        response->print("<tr><td colspan=\"2\" align=\"right\"><input type=\"submit\"></input></td></tr>");
        response->print("</table>");
        response->print("</div>");
        _footer(response);

        request->send(response);
    });
    this->server->on("/device.save", HTTP_POST, [this](AsyncWebServerRequest *request) {
        S0_LOG_DEBUG("Received POST to /device.save - reading parameters");
        
        // abort if no callback
        if (deviceFunc == 0) {
            S0_LOG_ERROR("No device callback set - nothing will be saved");
        } else {
            DeviceConfig cfg;

            // perform callback
            deviceFunc(&cfg);
        }

        // redirect
        request->redirect("/");
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

    S0_LOG_INFO("Constructed ConfigWebServer instance - starting to accept incoming requests");
    this->server->begin();
}

ConfigWebServer::~ConfigWebServer() {
    S0_LOG_INFO("Destructing ConfigWebServer instance - ending server and releasing memory");
    this->server->end();
    delete this->server;
    SPIFFS.end();
}

/*
void initWebserver(DeviceConfig *deviceCfg, WifiConfig *wifiCfg) {
    // save
    _wifiCfg = wifiCfg;
    _deviceCfg = deviceCfg;

    // get routes
    server.on("/", HTTP_GET, _get_root);
    server.on("/wificonfig.html", HTTP_GET, _get_wificonfig);
    server.on("/deviceconfig.html", HTTP_GET, _get_deviceconfig);
    server.on("/styles.css", HTTP_GET, _get_CSS);
    server.onNotFound(_notFound);

    // start
    server.begin();
}
*/


/*

void webRestarting(char* buffer) {
    char title[] = "Restarting";
    webHeader(buffer, false, title);
    strcat(buffer, "</body></html>");
}

void _notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}



void _get_root(AsyncWebServerRequest *request) {
    char data[] = \
    "<div class=\"position menuitem height30\"><a href=\"./wificonfig.html\">Wi-Fi Config.</a></div>\n" \
    "<div class=\"position menuitem height30\"><a href=\"./deviceconfig.html\">Device Config.</a></div>\n" \
    "<div class=\"position menuitem height30\"><a href=\"./data.html\">Data</a></div>\n" \
    
    
    "<div class=\"position menuitem height30\"><a href=\"./httpstatus.html\">HTTP status</a></div>";

    char response[1024];
    char title[] = "Menu";
    webHeader(response, false, title);
    strcat(response, data);
    strcat(response, "<div class=\"position footer right\">");
    strcat(response, VERSION_NUMBER);
    strcat(response, "<br/>");
    strcat(response, VERSION_LASTCHANGE);
    strcat(response, "</div>");
    strcat(response, "</body></html>");
    request->send(200, "text/html", response);
}



ConfigWebserver::ConfigWebserver(DeviceConfig *deviceCfg, WifiConfig *wifiCfg) {
    _wifiCfg = wifiCfg;
    _deviceCfg = deviceCfg;
}
void ConfigWebserver::init() {
    // add paths
    server.on("/", HTTP_GET, _get_root);
    server.on("/wificonfig.html", HTTP_GET, _get_wificonfig);
    server.on("/deviceconfig.html", HTTP_GET, _get_deviceconfig);
    server.on("/styles.css", HTTP_GET, _get_CSS);
    server.onNotFound(_notFound);
    server.begin();    
}



void webHandle_GetRoot() {
    char response[1024];
    webHeader(response, false, "Menu");
    strcat(response, "<div class=\"position menuitem height30\"><a href=\"./data.html\">Data</a></div><div class=\"position menuitem height30\"><a href=\"./sensorconfig.html\">Device/Sensor Config.</a></div><div class=\"position menuitem height30\"><a href=\"./wificonfig.html\">Wi-Fi Config.</a></div><div class=\"position menuitem height30\"><a href=\"./httpstatus.html\">HTTP status</a></div>");
    strcat(response, "<div class=\"position footer right\">");
    strcat(response, VERSION_NUMBER);
    strcat(response, "<br/>");
    strcat(response, VERSION_LASTCHANGE);
    strcat(response, "</div>");
    strcat(response, "</body></html>");
    server.send(200, "text/html", response);
}

void webHandle_GetHttpStatus() {
    char str_httpcode[8];
    sprintf(str_httpcode, "%d", lastHttpResponseCode);

    char response[600];
    webHeader(response, true, "HTTP Status");
    strcat(response, "<div class=\"position menuitem\">");
    strcat(response, "HTTP Code: ");
    strcat(response, str_httpcode);
    strcat(response, "<br/>");
    strcat(response, "HTTP Response: <br/>");
    strcat(response, lastHttpResponse);
    strcat(response, "<br/>");
    strcat(response, "</div>");
    strcat(response, "</body></html>");
    server.send(200, "text/html", response);
}

void webHandle_GetData() {
    char str_temp[8];
    char str_hum[8];
    uint8_t sensorCount = getSensorCount();

    char response[400 + sensorCount * 100];
    webHeader(response, true, "Data");
    strcat(response, "<div class=\"position menuitem\">");

    if (isSensorTypeDS18B20()) {
        if (sensorCount > 0) {
            for (uint8_t i = 0; i < sensorCount; i++) {
                dtostrf(sensorSamples[i], 6, TEMP_DECIMALS, str_temp);

                strcat(response, sensorIds[i]);
                strcat(response, ": ");
                strcat(response, str_temp);
                strcat(response, "<br/>");
            }
        } else {
            strcat(response, "No DS18B20 sensors found on bus");
        }
    } else if (isSensorTypeDHT22()) {
        dtostrf(sensorSamples[0], 6, TEMP_DECIMALS, str_temp);
        dtostrf(sensorSamples[1], 6, HUM_DECIMALS, str_hum);

        strcat(response, "Temperature: ");
        strcat(response, str_temp);
        strcat(response, "&deg;C<br/>Humidity: ");
        strcat(response, str_hum);
        strcat(response, "%");
    }

    strcat(response, "</div></body></html>");
    server.send(200, "text/html", response);
}

void webHandle_GetSensorConfig() {
    char str_deviceid[36];
    getMacAddressString(str_deviceid);
    char str_delay_print[12];
    sprintf(str_delay_print, "%lu", configuration.delayPrint);
    char str_delay_poll[12];
    sprintf(str_delay_poll, "%lu", configuration.delayPoll);
    char str_delay_post[12];
    sprintf(str_delay_post, "%lu", configuration.delayPost);

    // create buffer
    char response[2048];

    // show current response
    webHeader(response, true, "Device/Sensor Config.");
    strcat(response, "<div class=\"position menuitem\">");
    strcat(response, "<p>");
    strcat(response, "Device ID: ");
    strcat(response, str_deviceid);
    strcat(response, "<br/>");
    strcat(response, "Current delay print: ");
    strcat(response, str_delay_print);
    strcat(response, "ms<br/>");
    strcat(response, "Current delay poll: ");
    strcat(response, str_delay_poll);
    strcat(response, "ms<br/>");
    strcat(response, "Current delay post: ");
    strcat(response, str_delay_post);
    strcat(response, "ms<br/>");
    strcat(response, "Current endpoint: ");
    if (strcmp(configuration.endpoint, "") == 0) {
        strcat(response, "&lt;none configured&gt;");
    } else {
        strcat(response, configuration.endpoint);
    }
    strcat(response, "<br/>");
    strcat(response, "Current JWT: ");
    if (strcmp(configuration.jwt, "") == 0) {
        strcat(response, "&lt;none configured&gt;");
    } else {
        strncat(response, configuration.jwt, 15);
        strcat(response, "...");
    }
    strcat(response, "<br/>");
    strcat(response, "Current sensor type: ");
    strcat(response, configuration.sensorType);
    strcat(response, "<br/>");
    strcat(response, "</p>");

    // add form
    strcat(response, "<form method=\"post\" action=\"/sensor\">");
    strcat(response, "<table border=\"0\">");
    strcat(response, "<tr><td align=\"left\">Delay, print</td><td><input type=\"text\" name=\"print\" autocomplete=\"off\"></input></td></tr>");
    strcat(response, "<tr><td align=\"left\">Delay, poll</td><td><input type=\"text\" name=\"poll\" autocomplete=\"off\"></input></td></tr>");
    strcat(response, "<tr><td align=\"left\">Delay, post</td><td><input type=\"text\" name=\"post\" autocomplete=\"off\"></input></td></tr>");
    strcat(response, "<tr><td align=\"left\">Endpoint</td><td><input type=\"text\" name=\"endpoint\" autocomplete=\"off\"></input></td></tr>");
    strcat(response, "<tr><td align=\"left\">JWT</td><td><input type=\"text\" name=\"jwt\" autocomplete=\"off\"></input></td></tr>");
    strcat(response, "<tr><td align=\"left\">Sensor type</td><td><select name=\"sensortype\"><option>DS18B20</option><option>DHT22</option></select></td></tr>");
    strcat(response, "<tr><td colspan=\"2\" align=\"right\"><input type=\"submit\"></input></td></tr>");
    strcat(response, "</table>");

    // close page
    strcat(response, "</div></body></html>");
    server.send(200, "text/html", response);
}

void webHandle_PostSensorForm() {
    bool didUpdate = false;

    if (server.arg("print").length() > 0) {
        unsigned long larg = atol(server.arg("print").c_str());
        if (larg != 0) {
            configuration.delayPrint = larg;
            didUpdate = true;
        }
    }
    if (server.arg("poll").length() > 0) {
        unsigned long larg = atol(server.arg("poll").c_str());
        if (larg != 0) {
            configuration.delayPoll = larg;
            didUpdate = true;
        }
    }
    if (server.arg("post").length() > 0) {
        unsigned long larg = atol(server.arg("post").c_str());
        if (larg != 0) {
            configuration.delayPost = larg;
            didUpdate = true;
        }
    }
    if (server.arg("endpoint").length() > 0) {
        strcpy(configuration.endpoint, server.arg("endpoint").c_str());
        didUpdate = true;
    }
    if (server.arg("jwt").length() > 0) {
        strcpy(configuration.jwt, server.arg("jwt").c_str());
        didUpdate = true;
    }
    if (server.arg("sensortype").length() > 0) {
        strcpy(configuration.sensorType, server.arg("sensortype").c_str());
        didUpdate = true;
    }

    if (didUpdate) {
        // save to eeprom
        EEPROM.put(0, configuration);
        EEPROM.commit();

        // send response
        char response[400];
        webRestarting(response);
        server.send(200, "text/html", response);
        yield();

        // restart esp
        ESP.restart();
    }
}



void webHandle_PostWifiForm() {
    if (!server.hasArg("ssid") || !server.hasArg("password") || server.arg("ssid") == NULL || server.arg("password") == NULL) {
        server.send(417, "text/plain", "417: Invalid Request");
        return;
    }

    // save to eeprom
    server.arg("ssid").toCharArray(wifi_data.ssid, 20);
    server.arg("password").toCharArray(wifi_data.password, 20);
    wifi_data.keep_ap_on = (server.arg("keep_ap_on") && server.arg("keep_ap_on").charAt(0) == '1');
    EEPROM.put(sizeof configuration, wifi_data);
    EEPROM.commit();

    // send response
    char response[400];
    webRestarting(response);
    server.send(200, "text/html", response);
    delay(200);

    // restart esp
    ESP.restart();
}


*/