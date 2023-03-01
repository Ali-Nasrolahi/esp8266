/**
 * @file simple_scan.c
 * @author Ali Nasrolahi.
 * @brief Very simple wifi scan for ESP8266 SDK.
 * @date 2023-03-01
 *
 * @note This is very simple scanner using esp8266 chip.
 * Main references for this development is ESPRESSIF's ESP8266 & ESP32 official documentations and
 * SDK source codes.
 */

#include "esp_log.h"
#include "esp_wifi.h"
#include <string.h>

#define TAG "WIFI SCAN"
#define INFO(...) ESP_LOGI(TAG, ##__VA_ARGS__)
#define DELAY(_t) vTaskDelay(_t)
#define DELAY_IN_SEC(_t) DELAY((_t) / 100)

#define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE
#define DEFAULT_SCAN_INTERVAL CONFIG_EXAMPLE_TIME_INTERVAL

static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        INFO("Authmode \t\tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_WEP:
        INFO("Authmode \t\tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        INFO("Authmode \t\tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        INFO("Authmode \t\tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        INFO("Authmode \t\tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        INFO("Authmode \t\tWIFI_AUTH_WPA2_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        INFO("Authmode \t\tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        INFO("Authmode \t\tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    default:
        INFO("Authmode \t\tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    default:
        INFO("Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    default:
        INFO("Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

/**
 * @brief Prints ap_list's details.
 *
 * @param ap_list AP list.
 * @param ap_count AP counts.
 */
static void print_ap_details(wifi_ap_record_t *ap_list, uint16_t ap_count)
{
    for (int i = 0; i < DEFAULT_SCAN_LIST_SIZE && i < ap_count; i++) {
        INFO("Entry no.%d:\n", i);
        INFO("SSID \t\t%s", ap_list[i].ssid);
        INFO("RSSI \t\t%d", ap_list[i].rssi);
        INFO("Channel \t\t%d", ap_list[i].primary);

        print_auth_mode(ap_list[i].authmode);

        if (ap_list[i].authmode != WIFI_AUTH_WEP)
            print_cipher_type(ap_list[i].pairwise_cipher, ap_list[i].group_cipher);
        INFO("End OF ENTRY.\n");
    }
}

static void wifi_scan(void)
{
    /**
     *  @brief  Creates default event loop.
     *  @details The default event loop is a special type of loop used for system events (Wi-Fi
     * events, for example).
     * @note Check esp32's documentations for better understanding the concept of events;
     * specifically default event loop.
     */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /**
     * @brief Init WiFi Alloc resource for WiFi driver, such as WiFi control structure, RX/TX
     * buffer, WiFi NVS structure etc, this WiFi also start WiFi task.
     */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /**
     * @brief  we're initializing needed structures for each AP entry.
     * @details 'esp_wifi_scan' API presents each WIFI Access-points in 'wifi_ap_record' form.
     * @note 'wifi_apt_record_t' stores various AP's data like (SSID, RSSI, Chennal, etc).
     */
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    /* Setting Wifi to STA/Client mode*/
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    /**
     * @brief Start WiFi according to current configuration If mode is WIFI_MODE_STA, it create
     * station control block and start station.
     */
    ESP_ERROR_CHECK(esp_wifi_start());

    /// Scan all available APs.
    esp_wifi_scan_start(NULL, true);

    /// Get AP list found in last scan.
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

    /// Get number of APs found in last scan.
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

    INFO("Total APs scanned = %u\n", ap_count);

    print_ap_details(ap_info, ap_count);

    /* Cleanup */
    esp_wifi_scan_stop();
    esp_wifi_stop();
    esp_event_loop_delete_default();
}

void app_main(void)
{
    /// This will initialize TCPIP stack inside.
    tcpip_adapter_init();

    while (1) {
        wifi_scan();
        DELAY_IN_SEC(DEFAULT_SCAN_INTERVAL);
    }
}
