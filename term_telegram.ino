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
int loopCount = 0;
int64_t startedTime = 0;
int lastTemp = 0;
void loop()
{
  webServer.handleClient();
  if (WiFi.status() == WL_CONNECTED)
  {
    bot.tick(); // тикаем в луп
    delay(1);
    if (++loopCount > 1000)
    {
      loopCount = 0;
      startedTime++;
      if (startedTime >= 60 * 30) // 30 minuts
      {
        startedTime = 0;
        Serial.println();
        sensors.requestTemperatures();
        tempSensor1 = sensors.getTempC(sensor1); // Получить значение температуры
        if (tempSensor1 - lastTemp > 2)
        {
          lastTemp = tempSensor1;
        }
        else if (tempSensor1 < lastTemp)
        {
          lastTemp = tempSensor1;
          sprintf(buff, "Temperature [%s]: %s", name, String(tempSensor1, 1));
          bot.sendMessage(buff);
        }
        Serial.println("Temperature: " + String(tempSensor1, 1));
      }
    }
  }
}