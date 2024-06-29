#include <Arduino.h>

// Wifi connectivity
#include <WiFi.h>

// Timing
#include <elapsedMillis.h>

// Random 32 bit numbers and UUIDs
// #include <ESPRandom.h>

/* ---- Private Libraries ---- */
#include <Utilities.h>
#include <DIS.h>
#include <Scanning.h>
#include <DISBLE.h>
#include <DISNet.h>
/* ---- Private Libraries ---- */

#include <AES.h>
#include <Crypto.h>

class BLENodeDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  // Inheritance
  void onResult(BLEAdvertisedDevice);
};

void BLENodeDeviceCallbacks::onResult(BLEAdvertisedDevice device)
{
  if (!device.haveName())
    return;

  std::string device_name = device.getName();
  if (device_name.length() != 20)
  {
    Serial.println("Not a valid NAME length");
    return;
  }

  std::string device_name_start = device_name.substr(0, 3);
  if (device_name_start.compare("PMD") == !0)
  {
    Serial.printf("Not a valid NAME format");
    return;
  }

  Serial.printf("Device found.... %s\n", device_name.c_str());

  int service_count = device.getServiceUUIDCount();
  Serial.printf("Device has %d services\n", service_count);
  for (int i = 0; i < service_count; ++i)
  {
    Serial.printf("Service: %s \n", device.getServiceUUID(i).toString().c_str());
    Serial.printf("\\___:ADV Service?: %s \n", device.isAdvertisingService(device.getServiceUUID(i)) ? "t" : "f");
  }

  // TODO: transform the BLEAddress into a NODE
}

// ---------------------------------------------------

DeviceRoutingMode permissions = DeviceRoutingModes::receives + DeviceRoutingModes::transmits;

elapsedMillis timerOne;

AES128_ESP aes128;
MainBLEServer ble_server;
BLEScan *ble_scanner;

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

  // Wait for serial availability
  while (!Serial)
    ;

  DeviceIdentifier *dispIdentifier = new DeviceIdentifier(
      "PMD",
      RandomCharSequenceLen32(),
      DeviceOperationStates::good,
      permissions
      // -------------------------
  );

  dispIdentifier->SetAESKey(key);
  dispIdentifier->SetAESInstance(&aes128);
  dispIdentifier->GenerateSSID();

  // Empty for no password
  // const char *password = "";
  // dispIdentifier->SetApPassword(password);

  String bleId = "PMD";
  bleId.concat(dispIdentifier->GetSSID());
  bleId = bleId.substring(0, 20);

  BLEDevice::init(bleId.c_str());
  ble_server = MainBLEServer(new ServerCallbacks());

  // BLE device scanning
  ble_scanner = BLEDevice::getScan();
  ble_scanner->setAdvertisedDeviceCallbacks(new BLENodeDeviceCallbacks());
  ble_scanner->setActiveScan(true);
  ble_scanner->setInterval(1000);
  ble_scanner->setWindow(50);

  // Adicionar os callbacks à caracteristica de id do dispositivo -----------------
  BLECharacteristicCallbacks *pDeviceIdentifierCharacteristicCallbacks =
      new DidCallbacks(dispIdentifier, &ble_server);

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
      // ----
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

  // dispIdentifier->InitWifi();
  // BLENetworkNode usage
  // Node(BLEAdvertisedDevice *deviceId);
  BLEServer *server = ble_server.GetBleServerRef();
  BLENetworkNode *networkNode = new BLENetworkNode(server);
}

void loop()
{
  BLEScanResults results = ble_scanner->start(5, false);
}
