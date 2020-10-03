#include <ArduinoOTA.h>
#include <ESPmDNS.h>

#define ESP32_RTOS 1 // use the ota code with freertos (only works on the ESP32)

#if defined(ESP32_RTOS) && defined(ESP32)
void ota_handle(void* parameter) {
  for (;;) {
    ArduinoOTA.handle();
    delay(3500);
  }
}
#endif

void setupOTA(const char* hostname) {
    ArduinoOTA.setHostname(hostname);

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else  // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        S0_LOG_INFO("Start updating %s", type);
    });
    ArduinoOTA.onEnd([]() {
        S0_LOG_INFO("End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        S0_LOG_INFO("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        S0_LOG_ERROR("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            S0_LOG_ERROR("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            S0_LOG_ERROR("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            S0_LOG_ERROR("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            S0_LOG_ERROR("Receive Failed");
        else if (error == OTA_END_ERROR)
            S0_LOG_ERROR("End Failed");
    });

    ArduinoOTA.begin();

    // start mdns
    if (!MDNS.begin(hostname)) {
        S0_LOG_ERROR("Unable to start mdns");
    } else {
        S0_LOG_INFO("Started mdns with name: %s", hostname);
    }

    S0_LOG_INFO("OTA Initialized");

    #if defined(ESP32_RTOS) && defined(ESP32)
    S0_LOG_INFO("Creating freertos task to handle recurring task of handling updates");
    xTaskCreate(
        ota_handle,   /* Task function. */
        "OTA_HANDLE", /* String with name of task. */
        10000,        /* Stack size in bytes. */
        NULL,         /* Parameter passed as input of the task */
        1,            /* Priority of the task. */
        NULL);        /* Task handle. */
    #endif
}