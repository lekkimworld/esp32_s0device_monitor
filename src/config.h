#include <EEPROM.h>
#include "types.h"

extern RJ45 plugs[];
extern DeviceConfig deviceCfg;
extern WifiConfig wifiCfg;

int getDeviceConfigOffset() {
    return 0;
}

int getWifiConfigOffset() {
    return getDeviceConfigOffset() + sizeof(DeviceConfig);
}

int getRJ45ConfigOffset_Plug0() {
    return getWifiConfigOffset() + sizeof(WifiConfig);
}

int writeConfiguration(WifiConfig *cfg) {
    // ensure config version is set
    int offset = getDeviceConfigOffset();
    if (deviceCfg.version != CONFIGURATION_VERSION) {
        deviceCfg.version = CONFIGURATION_VERSION;
        strcpy(deviceCfg.jwt, "");
        strcpy(deviceCfg.endpoint, "");
        strcpy(deviceCfg.syslog_server, "");
        deviceCfg.syslog_port = 514;
        deviceCfg.productionCert = false;
        deviceCfg.useDisplay = true;
        EEPROM.put(offset, deviceCfg);

        offset = getRJ45ConfigOffset_Plug0();
        RJ45Config rj45cfg;
        strcpy(rj45cfg.name, "");
        EEPROM.put(offset, rj45cfg);
        offset += sizeof(RJ45Config);

        S0Config s0config[4];
        for (int i=0; i<4; i++) {
            strcpy(s0config[i].id, "");
            strcpy(s0config[i].name, "");    
            EEPROM.put(offset, s0config);
            offset += sizeof(S0Config);
        }

        strcpy(rj45cfg.name, "");
        EEPROM.put(offset, rj45cfg);
        offset += sizeof(RJ45Config);

        for (int i=0; i<4; i++) {
            strcpy(s0config[i].id, "");
            strcpy(s0config[i].name, "");    
            EEPROM.put(offset, s0config);
            offset += sizeof(S0Config);
        }
    }

    // write wifi config
    offset = getWifiConfigOffset();
    WifiConfig newCfg;
    newCfg.keep_ap_on = cfg->keep_ap_on;
    strcpy(newCfg.ssid, cfg->ssid);
    strcpy(newCfg.password, cfg->password);
    EEPROM.put(offset, newCfg);

    // move pointer
    wifiCfg = newCfg;

    // save and return
    EEPROM.commit();
    return 0;
}

int writeConfiguration(DeviceConfig *cfg) {
    int offset = getDeviceConfigOffset();

    DeviceConfig newCfg;
    strcpy(newCfg.endpoint, cfg->endpoint);
    strcpy(newCfg.jwt, cfg->jwt);
    newCfg.delay_post = cfg->delay_post;
    newCfg.productionCert = cfg->productionCert;
    newCfg.useDisplay = cfg->useDisplay;
    EEPROM.put(offset, newCfg);

    // move pointer
    deviceCfg = newCfg;

    // save and return
    EEPROM.commit();
    return 0;
}

/**
 * Reads the configuration from the flash and returns 0 if successful and 
 * 1 otherwise.
 */
int readConfiguration() {
    EEPROM.begin(
        sizeof(DeviceConfig) + 
        sizeof(WifiConfig) + 
        2 * sizeof(RJ45Config) + 
        8 * sizeof(S0Config)
    );

    // read device config
    EEPROM.get(getDeviceConfigOffset(), deviceCfg);
    
    // validate config
    if (deviceCfg.version != CONFIGURATION_VERSION) {
        // not a valid config
        S0_LOG_DEBUG("Couldn't find device config of required version (%d) in flash", CONFIGURATION_VERSION);
        return false;
    }
    S0_LOG_DEBUG("Read DEVICE configuration from flash - data follows");
    S0_LOG_DEBUG("Endpoint        : %s", deviceCfg.endpoint);
    S0_LOG_DEBUG("Production cert.: %s", deviceCfg.productionCert ? "true" : "false");
    S0_LOG_DEBUG("JWT             : %s", deviceCfg.jwt);
    S0_LOG_DEBUG("Syslog server   : %s", deviceCfg.syslog_server);
    S0_LOG_DEBUG("Syslog port     : %d", deviceCfg.syslog_port);
    S0_LOG_DEBUG("Delay post      : %u", deviceCfg.delay_post);

    // read wifi config
    EEPROM.get(getWifiConfigOffset(), wifiCfg);
    S0_LOG_DEBUG("Read WIFI configuration from flash - data follows");
    S0_LOG_DEBUG("SSID      : %s", wifiCfg.ssid);
    S0_LOG_DEBUG("Password  : %s", wifiCfg.password);
    S0_LOG_DEBUG("Keep AP on: %d", wifiCfg.keep_ap_on);

    // read config for first rj45 and then devices
    RJ45Config rj45cfg_0;
    S0Config s0config_0[4];
    int offset = getRJ45ConfigOffset_Plug0();
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
    plugs[0].activeDevices = 0;
    strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].name, s0config_0[DEVICE_IDX_ORANGE].name);
    strcpy(plugs[0].devices[DEVICE_IDX_ORANGE].id,   s0config_0[DEVICE_IDX_ORANGE].id);
    if (strlen(s0config_0[DEVICE_IDX_ORANGE].id) > 0) plugs[0].activeDevices++;
    strcpy(plugs[0].devices[DEVICE_IDX_BROWN].name,  s0config_0[DEVICE_IDX_BROWN].name);
    strcpy(plugs[0].devices[DEVICE_IDX_BROWN].id,    s0config_0[DEVICE_IDX_BROWN].id);
    if (strlen(s0config_0[DEVICE_IDX_BROWN].id) > 0) plugs[0].activeDevices++;
    strcpy(plugs[0].devices[DEVICE_IDX_BLUE].name,   s0config_0[DEVICE_IDX_BLUE].name);
    strcpy(plugs[0].devices[DEVICE_IDX_BLUE].id,     s0config_0[DEVICE_IDX_BLUE].id);
    if (strlen(s0config_0[DEVICE_IDX_BLUE].id) > 0) plugs[0].activeDevices++;
    strcpy(plugs[0].devices[DEVICE_IDX_GREEN].name,  s0config_0[DEVICE_IDX_GREEN].name);
    strcpy(plugs[0].devices[DEVICE_IDX_GREEN].id,    s0config_0[DEVICE_IDX_GREEN].id);
    if (strlen(s0config_0[DEVICE_IDX_GREEN].id) > 0) plugs[0].activeDevices++;

    strcpy(plugs[1].name, rj45cfg_1.name);
    plugs[1].activeDevices = 0;
    strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].name, s0config_1[DEVICE_IDX_ORANGE].name);
    strcpy(plugs[1].devices[DEVICE_IDX_ORANGE].id,   s0config_1[DEVICE_IDX_ORANGE].id);
    if (strlen(s0config_1[DEVICE_IDX_ORANGE].id) > 0) plugs[1].activeDevices++;
    strcpy(plugs[1].devices[DEVICE_IDX_BROWN].name,  s0config_1[DEVICE_IDX_BROWN].name);
    strcpy(plugs[1].devices[DEVICE_IDX_BROWN].id,    s0config_1[DEVICE_IDX_BROWN].id);
    if (strlen(s0config_1[DEVICE_IDX_BROWN].id) > 0) plugs[1].activeDevices++;
    strcpy(plugs[1].devices[DEVICE_IDX_BLUE].name,   s0config_1[DEVICE_IDX_BLUE].name);
    strcpy(plugs[1].devices[DEVICE_IDX_BLUE].id,     s0config_1[DEVICE_IDX_BLUE].id);
    if (strlen(s0config_1[DEVICE_IDX_BLUE].id) > 0) plugs[1].activeDevices++;
    strcpy(plugs[1].devices[DEVICE_IDX_GREEN].name,  s0config_1[DEVICE_IDX_GREEN].name);
    strcpy(plugs[1].devices[DEVICE_IDX_GREEN].id,    s0config_1[DEVICE_IDX_GREEN].id);
    if (strlen(s0config_1[DEVICE_IDX_GREEN].id) > 0) plugs[1].activeDevices++;

    // return
    return 0;
}
