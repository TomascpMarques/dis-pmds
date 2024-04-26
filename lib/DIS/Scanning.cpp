#include "Scanning.h"

#include <Arduino.h>
#include <WiFi.h>

void ScanPmdDevices()
{
    int networks_n = WiFi.scanNetworks();
    if (networks_n == 0)
    {
        Serial.printf("Found no networks!!!\n");
        return;
    }

    Serial.printf("Found %d general networks!!!\n", networks_n);
    for (int i = 0; i < networks_n; ++i)
    {
        String wifi_ssid = WiFi.SSID(i);

        if (!wifi_ssid.startsWith("DISPMD"))
            continue;

        Serial.printf("%d. %s - RSSI: %d\n", i + 1, WiFi.SSID(i), WiFi.RSSI(i));
    }
}