#pragma once

/* ---- BLE Libraries ---- */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <UUID.h>
/* ---- BLE Libraries ---- */

// #include <ESPRandom.h>

class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic);
    void onNotify(BLECharacteristic *pCharacteristic);
};

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
};

enum MainBLECharacteristics
{
    Identification = 0,
};

// Main BLE service - passive identification and data transmission
class MainBLEServer
{
    BLEServer *pServer = NULL;

    // BLE device identification service
    UUID identificationServiceUUID;
    UUID identificationCharacteristicUUID;
    BLECharacteristic *pIdentificationCharacteristic = NULL;
    BLEService *pIdentificationService = NULL;

    int connectedDeviceCount = 0;

public:
    /* ----------------------
    Constructors
    ------------------------ */
    MainBLEServer();
    MainBLEServer(
        BLECharacteristicCallbacks *idCharacteristicsCallbacks,
        BLEServerCallbacks *pServerCallbacks
        // ------
    );

    /* ----------------------
    Behavior
    ------------------------ */
    void StartIdentificationService();
    void StopIdentificationService();
    void StartAdvertising();

    BLECharacteristic *GetCharacteristic(MainBLECharacteristics characteristic) const;
};
