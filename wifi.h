#ifndef WIFI
#define WIFI
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "store.h"

namespace wifi
{
#define APname "Kai-term"
#define APpass "password"

    ESP8266WebServer webServer(80);

    void get_indexPage()
    {
        webServer.send(200, "text/html", htmlDocument);
    }

#define PARAMS_COUNT 5
    const String paramList[PARAMS_COUNT] = {"ssid", "password", "name", "temp", "trend"};
    int getParamCode(String param)
    {
        for (int i = 0; i < PARAMS_COUNT; i++)
            if (paramList[i] == param)
                return i;
        return -1;
    }

    void get_params()
    {
        int paramsNr = webServer.args();
        Store::Config cfg = Store::Read();
        String str = "";
        char buff[256];
        for (int i = 0; i < paramsNr; i++)
        {
            if (str.length() > 0)
                str.concat(",");
            switch (getParamCode(webServer.argName(i)))
            {
            case 0: // ssid
                sprintf(buff, "\"ssid\":\"%s\"", cfg.ssid);
                break;
            case 1: // password
                sprintf(buff, "\"password\":\"%s\"", cfg.pass);
                break;
            case 2: // name
                sprintf(buff, "\"name\":\"%s\"", cfg.name);
                break;
            case 3: // temp
                sprintf(buff, "\"temperature\":\"%s\"", String(tsens->temp(), 1));
                break;
            case 4: // trend
                sprintf(buff, "\"trend\":\"%s\"", tsens->trend());
                break;
            default:
                sprintf(buff, "\"%s\":\"unknown parameter\"", webServer.argName(i));
                break;
            }
            str.concat(buff);
        };
        sprintf(buff, "{%s}", str.c_str());
        webServer.send(200, "application/json", buff);
    }

    void put_params()
    {
        char buff[256];

        int paramsNr = webServer.args();
        Store::Config cfg = Store::Read();
        for (int i = 0; i < paramsNr; i++)
        {
            switch (getParamCode(webServer.argName(i)))
            {
            case 0: // ssid
                strcpy(cfg.ssid, webServer.arg(i).c_str());
                break;
            case 1: // password
                strcpy(cfg.pass, webServer.arg(i).c_str());
                break;
            case 2: // name
                strcpy(cfg.name, webServer.arg(i).c_str());
                name = cfg.name;
                break;
            }
        };
        Store::Write(cfg);
        cfg = Store::Read();
        sprintf(buff, "{\"name\":\"%s\",\"ssid\":\"%s\",\"password\":\"%s\"}", cfg.name, cfg.ssid, cfg.pass);
        TRACELN(buff);
        webServer.send(200, "application/json", buff);
    }

    void notFound()
    {
        webServer.send(404, "text/plain", "Not found");
    }

    void get_reset()
    {
        webServer.send(200, "text/plain", "Device restarting");
        resetFunc();
    }

    void CreateWebServer()
    {
        webServer.on("/", HTTP_GET, get_indexPage);
        webServer.on("/params", HTTP_GET, get_params);
        webServer.on("/params", HTTP_PUT, put_params);
        webServer.on("/reset", HTTP_GET, get_reset);
        webServer.onNotFound(notFound);
        webServer.begin();
        Serial.print("HTTP server start on ");
        Serial.println(serverIP);
    }

    void CreateAP()
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(APname, APpass);
        Serial.printf("WiFi configured as %s Password: %s\n", APname, APpass);

        serverIP = WiFi.softAPIP().toString();
    }

    bool Connect()
    {
        WiFi.hostname("ESP-device");
        int connectionAttempts = 0;
        Store::Config cfg = Store::Read();
        TRACE("Connect to: %s [pass: %s] ", cfg.ssid, cfg.pass);
        WiFi.begin(cfg.ssid, cfg.pass);
        while (WiFi.status() != WL_CONNECTED)
        {
            TRACE(".");
            if (++connectionAttempts >= 20)
            {
                TRACELN(" FAIL");
                blink();
                blink();
                blink();
                return false;
            }
            blink();
            delay(500);
        }
        TRACELN(" OK");
        serverIP = WiFi.localIP().toString();
        return true;
    }
}
#endif