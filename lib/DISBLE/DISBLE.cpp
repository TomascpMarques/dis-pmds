#include <DISBLE.h>

MainBLEServer::MainBLEServer() {}

MainBLEServer::MainBLEServer(
    BLECharacteristicCallbacks *idCharacteristicsCallbacks,
    BLEServerCallbacks *pServerCallbacks
    // ------
)
{
    // Criação dos UUID do Serviço de identificação e as sua caracteristica
    this->identificationServiceUUID.setVariant4Mode();
    this->identificationServiceUUID.generate();

    this->identificationCharacteristicUUID.setVariant4Mode();
    this->identificationCharacteristicUUID.generate();
    // -----------------------------------------------

    // Criação do servidor BLE
    this->pServer = BLEDevice::createServer();

    // Criação do serviço BLE de identificação
    this->pIdentificationService =
        this->pServer->createService(
            this->identificationServiceUUID.toCharArray()
            // -----------------------------------------
        );

    // Criação da caracteristica do serviço de identificação.
    uint32_t identificationCharacteristics =
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY;

    this->pIdentificationCharacteristic =
        this->pIdentificationService->createCharacteristic(
            this->identificationCharacteristicUUID.toCharArray(),
            identificationCharacteristics
            // --------------------------
        );

    // Define callbacks da caracteristica de identificação
    this->pIdentificationCharacteristic->setCallbacks(idCharacteristicsCallbacks);

    // Descriptor da caracteristica de identificação
    BLE2902 descriptor = BLE2902();
    descriptor.setValue("informação de identificação do dispositivo");

    this->pIdentificationCharacteristic->addDescriptor(&descriptor);

    this->pIdentificationService->start();
}

void MainBLEServer::StartIdentificationService()
{
    this->pIdentificationService->start();
}

void MainBLEServer::StopIdentificationService()
{
    this->pIdentificationService->stop();
}

void MainBLEServer::StartAdvertising()
{
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    // Registar advertising para o serviço de identificação
    pAdvertising->addServiceUUID(
        this->identificationServiceUUID.toCharArray());

    //
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);

    BLEDevice::startAdvertising();
}

BLECharacteristic *MainBLEServer::GetCharacteristic(MainBLECharacteristics characteristic) const
{
    switch (characteristic)
    {
    case MainBLECharacteristics::Identification:
        return this->pIdentificationCharacteristic;
    default:
        return nullptr;
    }
}

/*
=====================================================
   BLE identification service and server callbacks
=====================================================
*/

class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        Serial.printf("BLE - VALUE READ: %s\n", value);
    }
    void onNotify(BLECharacteristic *pCharacteristic)
    {
        Serial.printf("BLE NOTIFY VALUE\n");
    }
};

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        // Pass
    }
    void onDisconnect(BLEServer *pServer)
    {
        pServer->startAdvertising();
    }
};
