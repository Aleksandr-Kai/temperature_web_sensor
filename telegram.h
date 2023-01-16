#ifndef TELEGRAM
#define TELEGRAM
#include <FastBot.h>
#include "_telegram.h"
#include "store.h"

namespace TelegramBot
{
    FastBot bot(BOT_TOKEN);

    struct Command
    {
        String name;
        String value;
        String error;
    };

    Command ParseCommand(String str)
    {
        Command cmd;
        int spacePos = str.indexOf(' ');
        if (spacePos < 0)
            cmd.error = "Incorrect command";
        else
        {
            cmd.name = str.substring(0, spacePos);
            cmd.value = str.substring(spacePos + 1, str.length());
        }

        if (cmd.name == "" || cmd.value == "")
            cmd.error = "Incorrect format";

        return cmd;
    }

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

        // wifi::getParamCode(msg.text)
        if (msg.text == "/help")
        {
            bot.sendMessage(help);
        }
        else if (msg.text == "/temp")
        {
            char buff[256];

            sprintf(buff, "Температура [%s]: %s", name, String(tsens->temp(), 1));
            bot.sendMessage(buff);
            Serial.println(buff);
        }
        else if (msg.text == "/trend")
        {
            char buff[256];

            sprintf(buff, "Тренд [%s]: %s", name, tsens->trend());
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

    void init(String devName, String serverIP)
    {
        bot.setChatID(BOT_CHAT_ID);
        bot.attach(newMsg);
        bot.sendMessage("Подключен датчик [" + devName + "]  http://" + serverIP);
    }
}
#endif