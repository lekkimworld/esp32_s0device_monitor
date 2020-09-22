#pragma once
#include <ESPAsyncWebServer.h>
#include "types.h"

typedef std::function<void(WifiConfig *wifiCfg)> WifiConfigChangedCallback;
typedef std::function<void(DeviceConfig *deviceCfg)> DeviceConfigChangedCallback;

/**
 * Class to manage and control the web UI for configuration.
 */
class ConfigWebServer {
    private:
    WifiConfig *wifiCfg;
    DeviceConfig *deviceCfg;
    AsyncWebServer *server;
    WifiConfigChangedCallback wifiFunc;
    DeviceConfigChangedCallback deviceFunc;

    public:
    ConfigWebServer(DeviceConfig *deviceCfg, WifiConfig *wifiCfg);
    ~ConfigWebServer();
    void init();
    void setWifiChangedCallback(WifiConfigChangedCallback wifiFunc);
    void setDeviceChangedCallback(DeviceConfigChangedCallback wifiFunc);
};
