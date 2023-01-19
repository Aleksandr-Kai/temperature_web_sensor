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

    float trend()
    {
        float avg = 0;
        for (int pos = next(); pos != current; pos = next(pos))
        {
            if (buffer[pos] < -80)
                return 0;

            avg += buffer[next(pos)] - buffer[pos];
            Serial.printf("[%5s] ", String(buffer[pos], 1));
        }
        Serial.printf("[%5s] [TREND: %4s]\n", String(buffer[current], 1), String(avg, 1));
        return avg;
    }
};

class TSensor
{
private:
    DallasTemperature *sensors;
    uint8_t sensorAddr[8];
    unsigned long lastTime = millis();
    TRingBuffer *buff;
    bool trendFresh = false;
    float lastTrend;

public:
    void (*tempAlarm)() = NULL;

    TSensor(int buffSize = TLIST_SIZE)
    {
        OneWire *oneWire = new OneWire(ONE_WIRE_BUS);
        sensors = new DallasTemperature(oneWire);
        buff = new TRingBuffer(buffSize);
        buff->clear();
        sensors->begin(); // запуск библиотеки

        int deviceCount = sensors->getDeviceCount();
        if (deviceCount > 0)
        {
            DeviceAddress foundAddr;
            if (sensors->getAddress(foundAddr, 0))
            {
                for (int i = 0; i < 8; i++)
                {
                    sensorAddr[i] = foundAddr[i];
                }
            }
        }
    }

    void tick()
    {
        if (millis() - lastTime >= 10000)
        {
            sensors->requestTemperatures();
            buff->Set(sensors->getTempC(sensorAddr));
            lastTime = millis();
            if (tempAlarm != NULL)
            {
                tempAlarm();
            }
            trendFresh = false;
        }
    }

    float temp()
    {
        return buff->Get();
    }

    float trend()
    {
        if (!trendFresh)
        {
            lastTrend = buff->trend();
            trendFresh = true;
        }

        return lastTrend;
    }
};
#endif