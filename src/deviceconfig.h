#pragma once
#include <ESPAsyncWebServer.h>
#include "types.h"

typedef std::function<int (WifiConfig *wifiCfg)> WifiConfigChangedCallback;
typedef std::function<int (DeviceConfig *deviceCfg)> DeviceConfigChangedCallback;
typedef std::function<int (RJ45Config *plugCfgs, S0Config *s0Cfgs)> PlugConfigChangedCallback;

/**
 * Class to manage and control the web UI for configuration.
 */
class ConfigWebServer {
    private:
    AsyncWebServer *server;
    WifiConfigChangedCallback wifiFunc;
    DeviceConfigChangedCallback deviceFunc;
    PlugConfigChangedCallback plugFunc;
    void _deviceConfig();
    void _wifiConfig();
    void _plugConfig();

    public:
    ConfigWebServer();
    ~ConfigWebServer();
    void init();
    void setWifiChangedCallback(WifiConfigChangedCallback wifiFunc);
    void setDeviceChangedCallback(DeviceConfigChangedCallback wifiFunc);
    void setPlugChangedCallback(PlugConfigChangedCallback plugFunc);
};
