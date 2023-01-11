#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#include "common.h"

#include "temp.h"
#include "command.h"
#include "telegram.h"

#include "html.h"
#include "wifi.h"

//************************************************************************************

// Initialize
void setup()
{
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  delay(500);
  TRACELN();
  TRACELN("Start");

  Config cfg = GetWiFiConfig();
  name = cfg.name;
  TRACE("Device name [%s]\n", name);

  pinMode(STATUS_PIN, OUTPUT);
  delay(1);
  digitalWrite(STATUS_PIN, HIGH);

  DSSet();

  WiFi.hostname("ESP-device");
  ConnectTo();

  if (WiFi.status() == WL_CONNECTED)
  {
    TRACE("Connected, IP address: ");
    TRACELN(WiFi.localIP());

    bot.setChatID(BOT_CHAT_ID);
    bot.attach(newMsg);
  }
  else
  {
    CreateAP();
  }
  // CreateAP();
  CreateWebServer();
}

//************************************************************************************
void loop()
{
  webServer.handleClient();
  if (WiFi.status() == WL_CONNECTED)
  {
    tgloop();
  }
}