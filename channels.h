#ifndef CHANNELS
#define CHANNELS

namespace Channels
{
#define DOUT_COUNT 4

    class Pin
    {
    private:
        uint8_t _pin;
        bool _status;
        String _name;

    public:
        Pin(uint8_t dout_pin, bool status, String alias)
        {
            _pin = dout_pin;
            _status = status;
            _name = alias;

            pinMode(_pin, OUTPUT);
            delay(1);
            digitalWrite(_pin, status ? HIGH : LOW);
        }
        String StatusToString()
        {
            return _status ? "on" : "off";
        }
        bool GetStatus()
        {
            return _status;
        }
        void SetStatus(bool status)
        {
            _status = status;
            digitalWrite(_pin, status ? HIGH : LOW);
        }
        bool SetStatus(String status)
        {
            status.toLowerCase();
            if (status == "on")
            {
                SetStatus(true);
            }
            else if (status == "off")
            {
                SetStatus(false);
            }
            else
                return false;
            return true;
        }
        void Toggle()
        {
            SetStatus(!_status);
        }
        String Name()
        {
            return _name;
        }

        void Rename(String name)
        {
            _name = name;
        }
    };

    Pin **pinList;

    int DoutCount()
    {
        return DOUT_COUNT;
    }
    bool SetPin(String name, String status)
    {
        for (int i = 0; i < DOUT_COUNT; i++)
        {
            if (pinList[i]->Name() == name)
            {
                return pinList[i]->SetStatus(status);
            }
        }
        return false;
    }

    bool Rename(String name, String newName)
    {
        for (int i = 0; i < DOUT_COUNT; i++)
        {
            if (pinList[i]->Name() == name)
            {
                pinList[i]->Rename(name);
                return true;
            }
        }
        return false;
    }

    void Init()
    {
        pinList = (Pin **)calloc(DOUT_COUNT, sizeof(Pin *));

        pinList[0] = new Pin(D0, false, "CH_0");
        pinList[1] = new Pin(D1, false, "CH_1");
        pinList[2] = new Pin(D2, false, "CH_2");
        pinList[3] = new Pin(D3, false, "CH_3");
    }
}

#endif