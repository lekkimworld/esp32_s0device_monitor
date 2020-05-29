#include <ESPAsyncWebServer.h>
#include "types.h"

class ConfigWebServer {
    private:
    WifiConfig *wifiCfg;
    DeviceConfig *deviceCfg;
    AsyncWebServer *server;

    public:
    ConfigWebServer(DeviceConfig *deviceCfg, WifiConfig *wifiCfg);
    ~ConfigWebServer();
};
