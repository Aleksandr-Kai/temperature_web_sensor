#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer;
int deviceCount = 0;
uint8_t sensor1[8] = {0x28, 0xFF, 0x64, 0x1E, 0x5B, 0x86, 0x42, 0xAE};
float tempSensor1;

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

void DSSet()
{
    // запуск библиотеке
    sensors.begin();

    // определение устройств на шине
    deviceCount = sensors.getDeviceCount();
    TRACE("Found %i sensor(s)\n", deviceCount);

    TRACELN("Adress list");
    for (int i = 0; i < deviceCount; i++)
    {
        sensors.getAddress(Thermometer, i);
        TRACE("\tSensor-%i: ", i);
        printAddress(Thermometer);
    }

    sensors.requestTemperatures();
    tempSensor1 = sensors.getTempC(sensor1); // Получить значение температуры
    Serial.println("Temperature: " + String(tempSensor1, 3));
}