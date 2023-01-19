#include "common.h"
#include "channels.h"
#include "store.h"

#include "temp.h"
TSensor *tsens;

#include "telegram.h"

#include "html.h"
#include "wifi.h"

//************************************************************************************
int lastAlarmed = 0;
void tempAlarm()
{
  float trend = tsens->trend();
  if ((trend < -2 || trend > 2) && (lastAlarmed != trend))
  {
    lastAlarmed = trend;
    char buff[128];
    float temp = tsens->temp();
    sprintf(buff, "Attention! Temp: %s [Trend: %s]\n", String(temp, 1), String(trend, 1));
    Serial.println(buff);
    TelegramBot::Message(buff);
  }
}

// Initialize
void setup()
{
  Store::init();
  Serial.begin(115200);
  delay(500);
  TRACELN();
  TRACELN("Start");

  Channels::Init();

  tsens = new TSensor();
  tsens->tempAlarm = tempAlarm;
  // tsens->printSensors();

  Store::Config cfg = Store::Read();
  devName = cfg.name;
  TRACE("Device name [%s]\n", devName);

  pinMode(STATUS_PIN, OUTPUT);
  delay(1);
  digitalWrite(STATUS_PIN, HIGH);

  if (wifi::Connect())
  {
    TRACE("Connected, IP address: ");
    TRACELN(WiFi.localIP());

    TelegramBot::init(devName, serverIP);
  }
  else
  {
    wifi::CreateAP();
  }
  wifi::CreateWebServer();
}

//************************************************************************************
void loop()
{
  wifi::webServer.handleClient();
  if (WiFi.status() == WL_CONNECTED)
  {
    TelegramBot::bot.tick();
  }
  tsens->tick();
}