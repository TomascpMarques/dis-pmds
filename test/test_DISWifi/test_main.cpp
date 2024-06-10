#include <unity.h>
#include <Arduino.h>
#include <WString.h>
#include <AES.h>
#include <Crypto.h>

#include "DIS.h"

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_aes_ssid_creation()
{
    AES128 aes128;

    const byte key[16] = {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
        // -------------------
    };

    DeviceRoutingMode rmode = DeviceRoutingModes::receives + DeviceRoutingModes::transmits;
    DeviceIdentifier did = DeviceIdentifier(
        "PMD",
        RandomCharSequenceLen8(),
        DeviceOperationStates::good,
        rmode
        // -------------------------
    );

    String aesSSID = did.GenerateSSID();
    TEST_ASSERT_TRUE(aesSSID.length() == 16);
}

void test_aes_encryption()
{
    AES128 aes128;

    std::string key_utf8 = "Supper secret key 1234";
    byte *key_bytes;
    mempcpy(key_bytes, key_utf8.data(), key_utf8.length());

    std::string texto = "TEST";
    byte texto_bytes[16];
    mempcpy(texto_bytes, texto.data(), texto.length());

    byte cypher[16];
    byte textoDecrypt[16];

    aes128.setKey(key_bytes, 16);
    aes128.encryptBlock(cypher, texto_bytes);
}

void test_random_alphanumeric_seq_len32()
{
    std::string result = RandomCharSequenceLen32();
    int len_w = 32;
    int have = result.length();

    TEST_ASSERT_TRUE(len_w == have);
}

void test_random_alphanumeric_seq_len8()
{
    std::string result = RandomCharSequenceLen8();
    int len_w = 8;
    int have = result.length();

    TEST_ASSERT_TRUE(len_w == have);
}

/* void test_device_routing_mask_interpret()
{
    // Esta mascara iguala 5 |          0b100            |              0b001
    DeviceRoutingMode mask = DeviceRoutingModes::informs + DeviceRoutingModes::transmits;
    // Devolve um array
    DeviceRoutingModes *have = GetModesFromMask(&mask);
    DeviceRoutingModes want[] = {
        DeviceRoutingModes::informs,
        DeviceRoutingModes::transmits,
    };

    bool comparison_1 = want[0] == have[0];
    bool comparison_2 = want[1] == have[1];

    TEST_ASSERT_TRUE(comparison_1);
    TEST_ASSERT_TRUE(comparison_2);
} */

void test_new_device_ssid()
{
    char patient_id[8 + 1] = "aaaaaaaa";
    patient_id[8] = '\0';
    DeviceRoutingMode mode_r = DeviceRoutingModes::receives + DeviceRoutingModes::transmits;

    DeviceIdentifier ssid = DeviceIdentifier(
        "PMD",
        patient_id,
        DeviceOperationStates::good,
        mode_r);

    bool comparison = String(ssid.GetDeviceID()).equals("");
    TEST_ASSERT_FALSE(comparison);

    comparison = String(ssid.GetPatientID()).equals(patient_id);
    TEST_ASSERT_TRUE(comparison);
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();

    RUN_TEST(test_new_device_ssid);
    // RUN_TEST(test_device_routing_mask_interpret);

    RUN_TEST(test_random_alphanumeric_seq_len32);
    RUN_TEST(test_random_alphanumeric_seq_len8);

    RUN_TEST(test_aes_ssid_creation);

    UNITY_END();
}

void loop()
{
}
