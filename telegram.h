#include <FastBot.h>
#include "_telegram.h"

FastBot bot(BOT_TOKEN);

String help = "/temp - get temperature\n" +
              String("/ip - get web server IP\n");

// обработчик сообщений
void newMsg(FB_msg &msg)
{
    if (msg.chatID != BOT_CHAT_ID)
    { // Проверка доступа
        bot.sendMessage("Доступ запрещен", msg.chatID);
        return;
    }

    if (msg.text == "/help")
    {
        bot.sendMessage(help);
    }
    else if (msg.text == "/temp")
    {
        sensors.requestTemperatures();
        tempSensor1 = sensors.getTempC(sensor1); // Получить значение температуры
        sprintf(buff, "Температура [%s]: %s", name, String(tempSensor1, 1));
        bot.sendMessage(buff);
        Serial.println(buff);
    }
    else if (msg.text == "/ip")
    {
        bot.sendMessage("Адрес web сервера: http://" + serverIP);
    }

    // выводим всю информацию о сообщении
    // Serial.println(msg.toString());
    blink();
}

int loopCount = 0;
int64_t startedTime = 0;
int lastTemp = 0;

void tgloop()
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
                sprintf(buff, "Температура [%s]: %s", name, String(tempSensor1, 1));
                bot.sendMessage(buff);
            }
            Serial.println("Temperature: " + String(tempSensor1, 1));
        }
    }
}