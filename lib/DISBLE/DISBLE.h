#pragma once

/* ---- BLE Libraries ---- */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEUUID.h>
#include <BLE2902.h>
/* ---- BLE Libraries ---- */

#include <UUID.h>
#include <DIS.h>

enum MainBLECharacteristics
{
    DeviceID = 0,
    PatientID,
    DeviceState,
    RoutingMode,
};

// Main BLE service - passive identification and data transmission
class MainBLEServer
{
private:
    BLEServer *pServer = nullptr;
    BLEClient *pClient = nullptr;
    UUID uuidGen;

    // BLE device identification service
    char idServiceUUID[36 + 1];

    char deviceIdCharacteristicUUID[36 + 1];
    char patientIDCharacteristicUUID[36 + 1];
    char deviceStateCharacteristicUUID[36 + 1];
    char deviceRoutingCharacteristicUUID[36 + 1];

    BLECharacteristic *pDeviceIdCharacteristic = nullptr;
    BLECharacteristic *pPatientIDCharacteristic = nullptr;
    BLECharacteristic *pDeviceStateCharacteristic = nullptr;
    BLECharacteristic *pDeviceRoutingCharacteristic = nullptr;

    BLEService *pIdService = nullptr;
    // --------------------------------------------

public:
    /* ----------------------
    Constructors
    ------------------------ */
    MainBLEServer();
    MainBLEServer(
        BLEServerCallbacks *pServerCallbacks
        // ---------------------------------
    );

    /* ----------------------
    Setters
    ------------------------ */
    int SetCharacteristicCallbacks(MainBLECharacteristics characteristic, BLECharacteristicCallbacks *callbacksClass);
    int SetCharacteristicsCallbacks(BLECharacteristicCallbacks *callbacksClass, MainBLECharacteristics characteristic[], int charCount);

    /* ----------------------
    Behavior
    ------------------------ */
    void StartIdService();

    void StartIdentificationService();
    void StopIdentificationService();

    void StartAdvertising(String advertName);

    int SetCharacteristicValue(MainBLECharacteristics characteristic, String value, bool notify);

    BLECharacteristic *MapUUIDToCharacteristic(const char *uuid);
    BLECharacteristic *GetCharacteristic(MainBLECharacteristics characteristic) const;
    BLEUUID GetCharacteristicUUID(MainBLECharacteristics characteristic) const;
};

/*
----------------------------------
    DeviceIdentifier Callbacks
----------------------------------
*/

class DidCallbacks : public BLECharacteristicCallbacks
{
    // Class inheritance method implementations
    void onRead(BLECharacteristic *pCharacteristic);
    void onNotify(BLECharacteristic *pCharacteristic);

public:
    MainBLEServer *pBleServer;
    DeviceIdentifier *pDeviceIdentifier;

    DidCallbacks(DeviceIdentifier *pDeviceIdentifier, MainBLEServer *pBleServer);
};

/*
----------------------------------
        Server Callbacks
----------------------------------
*/

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
};
