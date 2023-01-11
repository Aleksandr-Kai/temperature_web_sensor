#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define APname "Kai-term"
#define APpass "password"

struct Config
{
    char ssid[32];
    char pass[32];
    char name[32];
};

Config GetWiFiConfig()
{
    Config cfg;
    EEPROM.get(0, cfg);
    return cfg;
}

void SetWiFiConfig(Config cfg)
{
    if (cfg.ssid == "")
        strcpy(cfg.ssid, APname);
    if (cfg.pass == "")
        strcpy(cfg.pass, APpass);
    EEPROM.put(0, cfg);
    EEPROM.commit();
}

ESP8266WebServer webServer(80);

void get_indexPage()
{
    webServer.send(200, "text/html", htmlDocument);
}

void get_params()
{
    sensors.requestTemperatures();
    tempSensor1 = sensors.getTempC(sensor1); // Получить значение температуры
    Config cfg = GetWiFiConfig();
    sprintf(buff, "{\"name\":\"%s\",\"ssid\":\"%s\",\"password\":\"%s\",\"temperature\":\"%s\"}", cfg.name, cfg.ssid, cfg.pass, String(tempSensor1, 1));
    webServer.send(200, "application/json", buff);
}

void put_params()
{
    int paramsNr = webServer.args();
    Config cfg;
    for (int i = 0; i < paramsNr; i++)
    {
        if (webServer.argName(i) == "ssid")
        {
            strcpy(cfg.ssid, webServer.arg(i).c_str());
        }
        else if (webServer.argName(i) == "password")
        {
            strcpy(cfg.pass, webServer.arg(i).c_str());
        }
        else if (webServer.argName(i) == "name")
        {
            strcpy(cfg.name, webServer.arg(i).c_str());
            name = cfg.name;
        }
    };
    SetWiFiConfig(cfg);
    cfg = GetWiFiConfig();
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

void get_temp()
{
    sensors.requestTemperatures();
    tempSensor1 = sensors.getTempC(sensor1); // Получить значение температуры
    sprintf(buff, "{\"temperature\":\"%s\"}", String(tempSensor1, 1));
    webServer.send(200, "application/json", buff);
}

void CreateWebServer()
{
    webServer.on("/", HTTP_GET, get_indexPage);
    webServer.on("/params", HTTP_GET, get_params);
    webServer.on("/params", HTTP_PUT, put_params);
    webServer.on("/reset", HTTP_GET, get_reset);
    webServer.on("/temp", HTTP_GET, get_temp);
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

bool ConnectTo()
{
    int connectionAttempts = 0;
    Config cfg = GetWiFiConfig();
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