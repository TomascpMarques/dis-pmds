#pragma once

#include <cinttypes>
#include <Utilities.h>
#include <WString.h>
#include <Crypto.h>
#include <AES.h>

/// @brief Binary representation to be used in MSB
enum DeviceOperationStates
{
    good = 0b0001,
    failure = 0b0010,
    warning = 0b0011,
    malfunctioning = 0b0100
};

/// @brief The default routing mode will be a mask set all to 0
///  then each specfied permission will set the corresponding bit to 1
///  allowing that functionality
enum DeviceRoutingModes
{
    informs = 0b001,
    receives = 0b010,
    transmits = 0b100,
};

// Como só existem 3 modos de routing, a ordem dos bits em MSB, seria
// informs, seguido de, receives, e finalizando transmits.
typedef uint8_t DeviceRoutingMode;

class DeviceIdentifier
{

private:
    // 16 byte - 128 bit AES key
    byte aesKey[16];
    byte aesSSID[16];
    const char *apPassword;
    AES128_ESP *aes128;

    String device_id;
    String patient_id;
    char device_prefix[3 + 1];

    DeviceOperationStates device_state;
    DeviceRoutingMode device_routing_mode;

public:
    /* ----------------------
    Constructors
    ------------------------ */
    DeviceIdentifier();

    DeviceIdentifier(
        char device_type[3 + 1],
        String patient_id,
        DeviceOperationStates operation_state = DeviceOperationStates::good,
        DeviceRoutingMode routing_modes = 0b000);

    /* ----------------------
    Getters
    ------------------------ */
    const char *GetDeviceID() const;
    const char *GetPatientID() const;
    const char *GetDeviceState() const;
    const char *GetRoutingMode() const;
    const char *GetDevicePrefix() const;
    const char *GetSSID() const;

    /* ----------------------
    Setter
    ------------------------ */
    void SetAESKey(const byte key[16]);
    void SetAESInstance(AES128_ESP *aes128);
    void SetApPassword(const char *password);

    /* ----------------------
    Behavior
    ------------------------ */
    const char *GenerateSSID();
    const char *DecryptSomeSSID(const byte cypher[16]);

    void InitWifi();
};
