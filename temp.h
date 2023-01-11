#ifndef TEMPSENSOR
#define TEMPSENSOR
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D4
#define KELVIN_ZERO -273.15
#define TLIST_SIZE 6

class TRingBuffer
{
private:
    float *buffer;
    int size;
    int current;

    int next(int from = -1)
    {
        from = (from < 0) ? current : from;
        return (from + 1 >= size) ? 0 : from + 1;
    }

    int prev(int from = -1)
    {
        from = (from < 0) ? current : from;
        return (from - 1 < 0) ? size - 1 : from - 1;
    }

public:
    TRingBuffer(int bufferSize)
    {
        buffer = (float *)calloc(bufferSize, sizeof(float));
        size = bufferSize;
        current = prev(0);
    }

    void Set(float value)
    {
        current = next();
        buffer[current] = value;
    }

    float Get()
    {
        return buffer[current];
    }

    void clear()
    {
        for (int i = 0; i < size; i++)
            buffer[i] = KELVIN_ZERO;
    }

    String trend()
    {
        float avg = 0;
        for (int pos = next(); pos != current; pos = next(pos))
        {
            if (buffer[pos] < -80)
                return "N/A";

            avg += buffer[next(pos)] - buffer[pos];
            Serial.printf("[%5s] ", String(buffer[pos], 1));
        }
        Serial.printf("[%5s] [TREND: %4s]\n", String(buffer[current], 1), String(avg, 1));
        return String(avg, 1);
    }
};

class TSensor
{
private:
    DallasTemperature *sensors;
    uint8_t sensor1[8] = {0x28, 0xFF, 0x64, 0x1E, 0x5B, 0x86, 0x42, 0xAE};
    unsigned long lastTime = millis();
    TRingBuffer *buff;

    void printAddress(DeviceAddress deviceAddress)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            Serial.print("0x");
            if (deviceAddress[i] < 0x10)
                Serial.print("0");
            Serial.print(deviceAddress[i], HEX);
            if (i < 7)
                Serial.print(", ");
        }
        Serial.println("");
    }

public:
    TSensor(int buffSize = TLIST_SIZE)
    {
        OneWire *oneWire = new OneWire(ONE_WIRE_BUS);
        sensors = new DallasTemperature(oneWire);
        buff = new TRingBuffer(buffSize);
        buff->clear();
        sensors->begin(); // запуск библиотеки
    }

    void printSensors()
    {
        int deviceCount = sensors->getDeviceCount();
        Serial.printf("Found %i sensor(s)\n", deviceCount);
        Serial.println("Adress list");
        DeviceAddress addr;
        for (int i = 0; i < deviceCount; i++)
        {
            if (!sensors->getAddress(addr, i))
            {
                Fatal("Can not get sensor addres");
            }
            Serial.printf("\tSensor-%i: ", i);
            printAddress(addr);
        }
    }

    void tick()
    {
        if (millis() - lastTime >= 10000)
        {
            sensors->requestTemperatures();
            buff->Set(sensors->getTempC(sensor1));
            lastTime = millis();
        }
    }

    void (*tempAlarm)() = NULL;

    float temp()
    {
        return buff->Get();
    }

    String trend()
    {
        return buff->trend();
    }
};
#endif