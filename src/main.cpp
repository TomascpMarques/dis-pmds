#include <Arduino.h>

// Wifi connectivity
#include <WiFi.h>

// Random 32 bit numbers and UUIDs
// #include <ESPRandom.h>

/* ---- Private Libraries ---- */
#include <Utilities.h>
#include <DIS.h>
#include <Scanning.h>
#include <DISBLE.h>
#include <AES.h>
#include <Crypto.h>
/* ---- Private Libraries ---- */

DeviceRoutingMode permissions = DeviceRoutingModes::receives + DeviceRoutingModes::transmits;

AES128_ESP aes128;
MainBLEServer ble_server;
int i = 0;

void setup()
{
  Serial.begin(115200);

  randomSeed(999999999);

  while (!Serial)
    ;

  BLEDevice::init("ESP32");
  ble_server = MainBLEServer(
      new CharacteristicCallbacks(),
      new ServerCallbacks()
      // --------
  );
  ble_server.StartAdvertising();

  const byte key[16] = {
      0x2b, 0x7e, 0x15, 0x16,
      0x28, 0xae, 0xd2, 0xa6,
      0xab, 0xf7, 0x15, 0x88,
      0x09, 0xcf, 0x4f, 0x3c
      // -------------------
  };

  DeviceIdentifier did = DeviceIdentifier(
      "PMD",
      RandomCharSequenceLen8(),
      DeviceOperationStates::good,
      permissions
      // ------------------------
  );

  did.SetAESKey(key);
  did.SetAESInstance(&aes128);

  const char *x = did.GenerateAESSSID();

  Serial.printf("SSID: %s", x);

  const char *aesSSID = x;

  did.InitWifi(aesSSID, "");
}

void loop()
{
  /* ScanPmdDevices(); */
  /*   BLECharacteristic *pIdentification = ble_server.GetCharacteristic(MainBLECharacteristics::Identification);

    char *value = "";
    sprintf(value, "TEST %d", i++);

    pIdentification->setValue(String(value).c_str());
    pIdentification->notify(); */

  delay(5000);
}