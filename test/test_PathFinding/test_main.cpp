#include <unity.h>
#include <WString.h>
#include <Arduino.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_new_device_ssid()
{
    class Node
    {
    private:
        String id;
        int distance;

    public:
        const char *getID()
        {
            return this->id.c_str();
        }
        const int getDistance()
        {
            return this->distance;
        }
    };

    sizeof(Node);

    TEST_ASSERT_TRUE(true);
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();

    RUN_TEST(test_new_device_ssid);

    UNITY_END();
}

void loop()
{
}