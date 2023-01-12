#ifndef STORE
#define STORE

#include <EEPROM.h>
#define EEPROM_SIZE 96 // save ssid & password & device_name

namespace Store
{
    struct Config
    {
        char ssid[16];
        char pass[16];
        char name[16];
        char valid[6];
    };

    void Write(Config &cfg)
    {
        EEPROM.put(0, cfg);
        EEPROM.commit();
    }

    Config Read()
    {
        Config cfg;
        EEPROM.get(0, cfg);
        if (strcmp(cfg.valid, "valid") != 0)
        {
            strcpy(cfg.name, "DevName");
            strcpy(cfg.ssid, "ssid");
            strcpy(cfg.pass, "password");
            strcpy(cfg.valid, "valid");
            Write(cfg);
        }
        return cfg;
    }

    void init()
    {
        EEPROM.begin(EEPROM_SIZE);
    }
}
#endif