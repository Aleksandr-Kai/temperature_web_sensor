void (*resetFunc)(void) = 0; // reboot device

void NullFunc() {}

void Fatal(String msg)
{
    Serial.println(msg);
    while (1)
    {
        /* code */
    }
}

#define STATUS_PIN D8 // led pin

// TRACE output simplified, can be deactivated here
#define TRACE(...) Serial.printf(__VA_ARGS__)
#define TRACELN(...) Serial.println(__VA_ARGS__)

void blink()
{
    digitalWrite(STATUS_PIN, HIGH);
    delay(50);
    digitalWrite(STATUS_PIN, LOW);
    delay(50);
}

void blink_long()
{
    digitalWrite(STATUS_PIN, HIGH);
    delay(100);
    digitalWrite(STATUS_PIN, LOW);
    delay(50);
}

String devName;
String serverIP;