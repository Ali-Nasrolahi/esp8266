#include <ESP8266WiFi.h>

#define DEFAULT_SCAN_INTERVAL 5000 /* Default is 5s */

#define PHY_MODE(_ap)                                                                              \
    (_ap)->phy_11n                                                                                 \
        ? F("802.11n")                                                                             \
        : ((_ap)->phy_11g ? F("802.11g") : ((_ap)->phy_11b ? F("802.11b") : F("UNKOWN")))

String ssid;
int32_t rssi;
uint8_t encryptionType;
uint8_t *bssid;
int32_t channel;
bool hidden;

/**
 * @brief Prints last scan info.
 *
 * @param scanResult number of found networks.
 */
void printInfo(uint32_t scanResult)
{
    for (int8_t i = 0; i < scanResult; i++) {
        WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

        const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
        Serial.printf(
            PSTR("  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %-11s %s\n"), i,
            channel, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi,
            (encryptionType == ENC_TYPE_NONE) ? ' ' : '*', hidden ? 'H' : 'V', PHY_MODE(bssInfo),
            ssid.c_str());
        yield();
    }
}
void setup()
{
    Serial.begin(74880);
    Serial.println(F("Simply Scan!!\n"));

    // Set WiFi to STA/Client mode
    WiFi.mode(WIFI_STA);

    delay(100);
}

void loop()
{
    int scanResult;
    Serial.println(PSTR("Start scanning!\n"));

    /* Prints networks' info */
    printInfo(WiFi.scanNetworks(false, true));

    Serial.println(PSTR("Done scanning!\n"));

    // Wait a bit before scanning again
    delay(DEFAULT_SCAN_INTERVAL);
}
