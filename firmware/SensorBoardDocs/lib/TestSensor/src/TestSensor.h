#ifndef TEST_SENSOR
#define TEST_SENSOR

#define uint16_t unsigned int

class TestSensor
{
    uint16_t data = 0;

public:
    TestSensor()
    {
        data = 0;
    }

    void read()
    {
        data++;
    }

    uint16_t get()
    {
        return data;
    }
};

# endif // TEST_SENSOR
