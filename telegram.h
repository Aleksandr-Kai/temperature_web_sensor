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