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
    char patient_id[8 + 1],
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

    strncpy(this->device_id, device_id, 9);
    strncpy(this->patient_id, patient_id, 9);

    this->device_state = operation_state;
    this->device_routing_mode = routing_modes;

    /* String state = "";
    if (operation_state == DeviceOperationStates::good)
    {
        state = "good";
    }
    else if (operation_state == DeviceOperationStates::malfunctioning)
    {
        state = "malf";
    }
    else if (operation_state == DeviceOperationStates::warning)
    {
        state = "warn";
    }
    else if (operation_state == DeviceOperationStates::failure)
    {
        state = "fail";
    }
    else
    {
        Serial.printf("Falha ao atribuir o estado do dispositivo");
        return;
    } */

    /* char routing_mode_binary[8 + 1];
    itoa(routing_modes, routing_mode_binary, 2);
    routing_mode_binary[8] = '\0'; */
}

DeviceIdentifier::DeviceIdentifier()
{
}

/// @brief Construtor que cria um DeviceWiFiSSID a partir de uma string.
/// @param hashed_id uma string de caracteres de tamanho 32.
/* DeviceIdentifier::DeviceIdentifier(String hashed_id)
{
    if (!hashed_id.startsWith("DISPMD"))
    {
        Serial.printf("Falha ao extrair o prefixo do dispositivo com o SSID fornecido.\n");
        return;
    }
    else if (hashed_id.length() != 32)
    {
        Serial.printf("O SSID fornecido é de tamanho ilegal (!= 32).\n");
        return;
    }

    // Extract device prefix
    hashed_id.toCharArray(this->device_prefix, 3 + 1, 3);
    this->device_prefix[4] = '\0';

    // Extract device_id
    hashed_id.toCharArray(this->device_id, 8 + 1, 6);
    this->device_id[8] = '\0';

    // Extract patient_id
    hashed_id.toCharArray(this->patient_id, 8 + 1, 8 + 7);
    this->patient_id[8] = '\0';

    // Full hashed_id string
    // hashed_id.toCharArray(this->patient_id, 32 + 1, 0);

    // Match device state
    String device_state = hashed_id.substring(24, 28);
    if (device_state.equals("good"))
    {
        this->device_state = DeviceOperationStates::good;
    }
    else if (device_state.equals("malf"))
    {
        this->device_state = DeviceOperationStates::malfunctioning;
    }
    else if (device_state.equals("fail"))
    {
        this->device_state = DeviceOperationStates::failure;
    }
    else if (device_state.equals("warn"))
    {
        this->device_state = DeviceOperationStates::warning;
    }
    else
    {
        Serial.printf("Falha ao extrair o estado do dispositivo: <%s>\n", device_state);
        return;
    }

    // Match device routing setup
    char routing_mode[3] = {0, 0, 0};
    hashed_id.substring(29, 32).toCharArray(routing_mode, 4);

    for (int i = 0; i < 3; ++i)
        if (!isDigit(routing_mode[i]))
        {
            Serial.printf("Falha ao extrair modo de routing.\n");
            return;
        }

    uint8_t mode = (uint8_t)strtol(routing_mode, NULL, 2);
    this->device_routing_mode = mode;
} */

/// @brief Devolve o id da máquina
/// @return const char*
const char *DeviceIdentifier::GetDeviceID() const
{
    return this->device_id;
}

/// @brief Devolve o id do paciente que o dispositivo está associado
/// @return const char*
const char *DeviceIdentifier::GetPatientID() const
{
    return this->patient_id;
}

/// @brief Devolve o prefixo do dispositivo incluido no ID
/// @return uma string com o valor do prefixo
const char *DeviceIdentifier::GetDevicePrefix() const
{
    return this->device_prefix;
}

const char *DeviceIdentifier::GetSSIDAES() const
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

const char *DeviceIdentifier::GenerateAESSSID()
{
    this->aes128->setKey(this->aesKey, 16);

    byte source_bytes[16] = {};

    const char *id = RandomCharSequenceLen12();
    std::string source_text = this->device_prefix;
    Serial.printf("DEVICE PREFIX: %s \n", device_prefix);
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

void DeviceIdentifier::InitWifi(const char hashed_id[32], const char *password)
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(hashed_id, strcmp(password, "") ? NULL : password);
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
