#include "common.h"

#include "store.h"

#include "temp.h"
TSensor *tsens;

#include "telegram.h"

#include "html.h"
#include "wifi.h"


//************************************************************************************

// Initialize
void setup()
{
  Store::init();
  Serial.begin(115200);
  delay(500);
  TRACELN();
  TRACELN("Start");

  tsens = new TSensor();
  // tsens->printSensors();

  Store::Config cfg = Store::Read();
  name = cfg.name;
  TRACE("Device name [%s]\n", name);

  pinMode(STATUS_PIN, OUTPUT);
  delay(1);
  digitalWrite(STATUS_PIN, HIGH);

  if (wifi::Connect())
  {
    TRACE("Connected, IP address: ");
    TRACELN(WiFi.localIP());

    TelegramBot::init(name, serverIP);
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