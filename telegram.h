#ifndef TELEGRAM
#define TELEGRAM
#include <FastBot.h>
#include "_telegram.h"
#include "store.h"

namespace TelegramBot
{
#define ALARM_PIN D7

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
    int32_t menuID = 0;
    String getTelegramMenu()
    {
        String menu = "";

        int col = 0;
        for (int i = 0; i < Channels::DoutCount(); i++)
        {
            menu += Channels::pinList[i]->Name();
            menu += Channels::pinList[i]->GetStatus() ? " off" : " on";
            if (++col == 2)
            {
                menu += "\n";
                col = 0;
            }
            else
                menu += "\t";
        }

        menu += "get temp\nget trend\nget ip";

        return menu; // menu.substring(0, menu.length());
    }

    void updateTelegramMenu()
    {
        if (menuID == 0)
        {
            bot.inlineMenu(devName, getTelegramMenu());
            menuID = bot.lastBotMsg();
        }
        else
        {
            bot.editMenu(menuID, getTelegramMenu());
        }
    }

    // обработчик сообщений
    void newMsg(FB_msg &msg)
    {
        if (msg.chatID != BOT_CHAT_ID)
        { // Проверка доступа
            bot.sendMessage("Доступ запрещен", msg.chatID);
            return;
        }

        if (msg.text == "/menu")
        {
            Serial.println("Menu request");
            menuID = 0;
            updateTelegramMenu();
            return;
        }

        if (msg.text == devName)
        {
            Command cmd = ParseCommand(msg.data);
            if (cmd.error != "")
            {
                bot.sendMessage(cmd.error);
                return;
            }
            if (cmd.name == "get")
            {
                if (cmd.value == "temp")
                {
                    char buff[256];

                    sprintf(buff, "Температура [%s]: %s", devName, String(tsens->temp(), 1));
                    bot.sendMessage(buff);
                    Serial.println(buff);
                }
                else if (cmd.value == "trend")
                {
                    char buff[256];

                    sprintf(buff, "Тренд [%s]: %s", devName, String(tsens->trend(), 1));
                    bot.sendMessage(buff);
                    Serial.println(buff);
                }
                else if (cmd.value == "ip")
                {
                    bot.sendMessage("Локальный сервер [" + devName + "]: http://" + serverIP);
                }
            }
            else
            {
                Serial.printf("SET <%s=%s>\n", cmd.name, cmd.value);
                if (!Channels::SetPin(cmd.name, cmd.value))
                {
                    bot.sendMessage("Не удалось установить параметр");
                    return;
                }
                updateTelegramMenu();
            }
        }
        blink();
    }

    void init(String devName, String serverIP)
    {
        bot.setChatID(BOT_CHAT_ID);
        bot.attach(newMsg);
        bot.sendMessage("Подключен датчик [" + devName + "]  http://" + serverIP);
        pinMode(ALARM_PIN, OUTPUT);
        updateTelegramMenu();
    }
    void Message(String msg)
    {
        bot.sendMessage(msg);
    }
}
#endif