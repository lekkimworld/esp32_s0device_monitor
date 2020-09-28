#include <EEPROM.h>
#include "types.h"

extern RJ45 plugs_runtime[];
extern RJ45Config rj45config[];
extern S0Config s0config[];
extern DeviceConfig deviceconfig;
extern WifiConfig wificonfig;

int getDeviceConfigOffset() {
    return 0;
}

int getWifiConfigOffset() {
    return getDeviceConfigOffset() + sizeof(DeviceConfig);
}

int getRJ45ConfigOffset_Plug0() {
    return getWifiConfigOffset() + sizeof(WifiConfig);
}

int initConfiguration() {
    EEPROM.begin(
        sizeof(DeviceConfig) + 
        sizeof(WifiConfig) + 
        2 * sizeof(RJ45) + 
        8 * sizeof(S0Device)
    );

    DeviceConfig eepromdeviceconfig;
    EEPROM.get(getDeviceConfigOffset(), eepromdeviceconfig);
    if (eepromdeviceconfig.version == CONFIGURATION_VERSION) {
        S0_LOG_INFO("Configuration in flash at latest version - will NOT reset it");
        return 1;
    }
    S0_LOG_INFO("Configuration in flash NOT at latest version (%d) - will reset it", eepromdeviceconfig.version);

    // reset wifi config
    int offset = getWifiConfigOffset();
    strcpy(wificonfig.ssid, "");
    strcpy(wificonfig.password, "");
    wificonfig.keep_ap_on = false;
    EEPROM.put(offset, wificonfig);

    // reset device config
    offset = getDeviceConfigOffset();
    deviceconfig.version = CONFIGURATION_VERSION;
    strcpy(deviceconfig.jwt, "");
    strcpy(deviceconfig.endpoint, "");
    strcpy(deviceconfig.syslog_server, "");
    deviceconfig.syslog_port = 514;
    deviceconfig.productionCert = false;
    deviceconfig.useDisplay = true;
    EEPROM.put(offset, deviceconfig);
        
    // reset plug config
    offset = getRJ45ConfigOffset_Plug0();
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
        strcpy(plugs_runtime[i].name, "");
        plugs_runtime[i].activeDevices = 0;
        strcpy(rj45config[i].name, "");
        EEPROM.put(offset, plugs_runtime[i]);
        offset += sizeof(RJ45Config);

        for (int k=0; k<DEVICES_PER_PLUG; k++) {
            uint8_t idx = i * DEVICES_PER_PLUG + k;
            strcpy(plugs_runtime[i].devices[k].id, "");
            strcpy(plugs_runtime[i].devices[k].name, "");
            strcpy(s0config[idx].id, "");
            strcpy(s0config[idx].name, "");
            EEPROM.put(offset, plugs_runtime[i].devices[k]);
            offset += sizeof(S0Config);
        }
    }
    
    // commit
    EEPROM.commit();
    return 0;
}

int writeConfiguration(WifiConfig *cfg) {
    // write wifi config
    int offset = getWifiConfigOffset();
    wificonfig.keep_ap_on = cfg->keep_ap_on;
    strcpy(wificonfig.ssid, cfg->ssid);
    strcpy(wificonfig.password, cfg->password);
    EEPROM.put(offset, wificonfig);
    
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
    deviceconfig = newCfg;

    // save and return
    EEPROM.commit();
    return 0;
}

int writeConfiguration(RJ45Config *newRJ45, S0Config *newS0) {
    // copy data into runtime versions
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
        strcpy(plugs_runtime[i].name, newRJ45[i].name);
        strcpy(rj45config[i].name, newRJ45[i].name);
        plugs_runtime[i].activeDevices = 0;

        for (int k=0; k<DEVICES_PER_PLUG; k++) {
            uint8_t idx = i * DEVICES_PER_PLUG + k;
            strcpy(s0config[idx].id, newS0[idx].id);
            strcpy(s0config[idx].name, newS0[idx].name);

            strcpy(plugs_runtime[i].devices[k].id, newS0[idx].id);
            strcpy(plugs_runtime[i].devices[k].name, newS0[idx].name);
            if (strlen(s0config[idx].id) > 0) plugs_runtime[i].activeDevices++;
        }
    }

    int offset = getRJ45ConfigOffset_Plug0();
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
        EEPROM.put(offset, rj45config[i]);
        offset += sizeof(RJ45Config);

        for (int k=0; k<DEVICES_PER_PLUG; k++) {
            uint8_t idx = i * DEVICES_PER_PLUG + k;
            EEPROM.put(offset, s0config[idx]);
            offset += sizeof(S0Config);
        }
    }
    
    // return
    EEPROM.commit();
    return 0;
}

/**
 * Reads the configuration from the flash and returns 0 if successful and 
 * 1 otherwise.
 */
int readConfiguration() {
    // read device config
    EEPROM.get(getDeviceConfigOffset(), deviceconfig);
    
    // validate config
    if (deviceconfig.version != CONFIGURATION_VERSION) {
        // not a valid config
        S0_LOG_DEBUG("Couldn't find device config of required version (%d) in flash", CONFIGURATION_VERSION);
        return false;
    }
    S0_LOG_DEBUG("Read DEVICE configuration from flash - data follows");
    S0_LOG_DEBUG("Endpoint        : %s", deviceconfig.endpoint);
    S0_LOG_DEBUG("Production cert.: %s", deviceconfig.productionCert ? "true" : "false");
    S0_LOG_DEBUG("JWT             : %s", deviceconfig.jwt);
    S0_LOG_DEBUG("Syslog server   : %s", deviceconfig.syslog_server);
    S0_LOG_DEBUG("Syslog port     : %d", deviceconfig.syslog_port);
    S0_LOG_DEBUG("Delay post      : %u", deviceconfig.delay_post);

    // read wifi config
    EEPROM.get(getWifiConfigOffset(), wificonfig);
    S0_LOG_DEBUG("Read WIFI configuration from flash - data follows");
    S0_LOG_DEBUG("SSID      : %s", wificonfig.ssid);
    S0_LOG_DEBUG("Password  : %s", wificonfig.password);
    S0_LOG_DEBUG("Keep AP on: %d", wificonfig.keep_ap_on);

    // read config for first rj45 and then devices
    int offset = getRJ45ConfigOffset_Plug0();
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
        EEPROM.get(offset, rj45config[i]);
        offset += sizeof(RJ45Config);

        for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
            uint8_t idx = i * DEVICES_PER_PLUG + k;
            EEPROM.get(offset, s0config[idx]);
            offset += sizeof(S0Config);
        }
    }

    // move config into runtime config
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
        strcpy(plugs_runtime[i].name, rj45config[i].name);

        for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
            uint8_t idx = i * DEVICES_PER_PLUG + k;
            strcpy(plugs_runtime[i].devices[k].id, s0config[idx].id);
            strcpy(plugs_runtime[i].devices[k].name, s0config[idx].name);
            if (strlen(s0config[idx].id) > 0) plugs_runtime[i].activeDevices++;
        }
    }

    // show plug config
    S0_LOG_DEBUG("Read PLUG configuration from flash - data follows");
    for (uint8_t i=0; i<RJ45_PLUG_COUNT; i++) {
        S0_LOG_DEBUG("Plug %d, name: %s", i, rj45config[i].name);

        for (uint8_t k=0; k<DEVICES_PER_PLUG; k++) {
            uint8_t idx = i * DEVICES_PER_PLUG + k;
            S0_LOG_DEBUG("Plug %d, device %d, id  : %s", i, k, s0config[idx].id);
            S0_LOG_DEBUG("Plug %d, device %d, name: %s", i, k, s0config[idx].name);
        }
    }

    // return
    return 0;
}
