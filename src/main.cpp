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

const byte key[16] = {
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88,
    0x09, 0xcf, 0x4f, 0x3c
    // -------------------
};

void setup()
{
  Serial.begin(115200);

  randomSeed(999999999);

  // Wait for serial availability
  while (!Serial)
    ;

  DeviceIdentifier *did = new DeviceIdentifier(
      "PMD",
      RandomCharSequenceLen8(),
      DeviceOperationStates::good,
      permissions
      // -------------------------
  );

  did->SetAESKey(key);
  did->SetAESInstance(&aes128);
  did->GenerateAESSSID();

  // Empty for no password
  did->SetApPassword("");

  String bleId = "PMD";
  bleId.concat(did->GetSSIDAES());
  bleId = bleId.substring(0, 20);

  BLEDevice::init(bleId.c_str());
  ble_server = MainBLEServer(new ServerCallbacks());

  // Adicionar os callbacks à caracteristica de id do dispositivo -----------------
  BLECharacteristicCallbacks *pDeviceIdentifierCharacteristicCallbacks =
      new DidCallbacks(did, &ble_server);

  /* int err = ble_server.SetCharacteristicCallbacks(
      MainBLECharacteristics::PatientID,
      patientIdCharacteristicCallbacks
  ); */

  MainBLECharacteristics caracteristicas[4] = {
      MainBLECharacteristics::PatientID,
      MainBLECharacteristics::DeviceID,
      MainBLECharacteristics::DeviceState,
      MainBLECharacteristics::RoutingMode
      // ---------------------------------
  };

  int err = ble_server.SetCharacteristicsCallbacks(
      pDeviceIdentifierCharacteristicCallbacks,
      caracteristicas,
      4
      //
  );
  if (err != 0)
  {
    Serial.printf("FATAL: FAILED TO ADD CALLBACKS TO DEVICE ID CHARACTERISTICS\n");
    exit(-1);
  }
  delay(100);
  // ------------------------------------------------------------------------------

  ble_server.StartIdService();
  delay(100);
  ble_server.StartAdvertising(bleId);

  did->InitWifi();
}

void loop()
{
}