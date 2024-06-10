#include <cstdint>
#include <Arduino.h>
#include <Utilities.h>
#include <string>

#include "DIS.h"
#include <AES.h>
#include <Crypto.h>
#include <WiFi.h>

/// @date 19/04/24
/// @author Tomás Marques
/// @brief Creates a new SSID for use in a DIS device, differentiating the device from others in the network
/// @param operation_state The operation mode of the device
/// @param device_type Char array indicating type of device.
/// @param patient_id Char array indicating the current patient's ID.
/// @param routing_mode An array receiving network routing behavior.
DeviceIdentifier::DeviceIdentifier(
    char device_type[3 + 1],
    String patient_uuid,
    DeviceOperationStates operation_state,
    DeviceRoutingMode routing_modes)
{
    if (routing_modes > 7)
    {
        Serial.printf("Metodo de routing inválido");
        return;
    }

    strncpy(this->device_prefix, device_type, 4);

    char hashed_id[32 + 1] = "";
    char *device_id = RandomCharSequenceLen8();

    this->device_id = device_id;
    this->patient_id = patient_uuid.c_str();

    this->device_state = operation_state;
    this->device_routing_mode = routing_modes;
}

DeviceIdentifier::DeviceIdentifier()
{
}

/// @brief Devolve o id da máquina
/// @return const char*
const char *DeviceIdentifier::GetDeviceID() const
{
    return this->device_id.c_str();
}

/// @brief Devolve o id do paciente que o dispositivo está associado
/// @return const char*
const char *DeviceIdentifier::GetPatientID() const
{
    return this->patient_id.c_str();
}

const char *DeviceIdentifier::GetDeviceState() const
{
    switch (this->device_state)
    {
    case DeviceOperationStates::good:
        return "good";
    case DeviceOperationStates::failure:
        return "failure";
    case DeviceOperationStates::malfunctioning:
        return "malfunctioning";
    case DeviceOperationStates::warning:
        return "warning";
    }
}

const char *DeviceIdentifier::GetRoutingMode() const
{
    switch (this->device_routing_mode)
    {
    case DeviceRoutingModes::informs:
        return "informs";
    case DeviceRoutingModes::receives:
        return "receives";
    case DeviceRoutingModes::transmits:
        return "transmits";
    }
}

/// @brief Devolve o prefixo do dispositivo incluido no ID
/// @return uma string com o valor do prefixo
const char *DeviceIdentifier::GetDevicePrefix() const
{
    return this->device_prefix;
}

const char *DeviceIdentifier::GetSSID() const
{
    static String crypt = "";
    for (int i = 0; i < 16; i++)
    {
        char str[2 + 1];

        sprintf(str, "%02x", (int)this->aesSSID[i]);
        crypt.concat(str);
    }

    return crypt.c_str();
}

void DeviceIdentifier::SetAESKey(const byte key[16])
{
    memcpy(this->aesKey, key, 16);
}

void DeviceIdentifier::SetAESInstance(AES128_ESP *aes128)
{
    this->aes128 = aes128;
}

void DeviceIdentifier::SetApPassword(const char *password)
{
    this->apPassword = password;
}

const char *DeviceIdentifier::GenerateSSID()
{
    this->aes128->setKey(this->aesKey, 16);

    byte source_bytes[16] = {};

    const char *id = RandomCharSequenceLen12();
    std::string source_text = this->device_prefix;
    source_text.append(id);

    Serial.printf("SOURCE TEXT: %s\n", source_text.c_str());

    for (int i = 0; i < 16; ++i)
        source_bytes[i] = (byte)source_text[i];

    source_bytes[15] = '\0';

    byte cypher[16];
    aes128->encryptBlock(cypher, source_bytes);
    memcpy(this->aesSSID, cypher, 16);

    static String crypt = "";
    for (int i = 0; i < 16; i++)
    {
        char str[2 + 1];

        sprintf(str, "%02x", (int)cypher[i]);
        crypt.concat(str);
    }

    return crypt.c_str();
}

const char *DeviceIdentifier::DecryptSomeSSID(const byte cypher[16])
{
    static String decrypted_text_utf8 = "";
    byte bytes[16];

    this->aes128->decryptBlock(bytes, cypher);

    for (int i = 0; i < 16; ++i)
    {
        char str[2];

        sprintf(str, "%c", (char)bytes[i]);
        decrypted_text_utf8.concat(str);
    }

    return decrypted_text_utf8.c_str();
}

void DeviceIdentifier::InitWifi()
{
    const char *ssid = this->GetSSID();

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, strcmp(this->apPassword, "") ? NULL : this->apPassword);
}

/// @brief Devolve um array com os modos de routing incluidos na mascara
/// @param mask
/// @return Array com os modos de routing associados à mascara
DeviceRoutingModes *GetModesFromMask(DeviceRoutingMode *mask)
{
    int i = 0;
    static DeviceRoutingModes modes[3] = {};

    if (bitRead(*mask, 0) == 1)
        modes[i++] = DeviceRoutingModes::informs;

    if (bitRead(*mask, 1) == 1)
        modes[i++] = DeviceRoutingModes::receives;

    if (bitRead(*mask, 2) == 1)
        modes[i++] = DeviceRoutingModes::transmits;

    return modes;
}
