#include <iostream>
// 组件的头文件
#include "wifi.h"
#include "suwp_type.h"

// esp-idf的头文件
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

static esp_netif_t *wifi_ap_netif;
static esp_netif_t *wifi_sta_netif;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_STA_START)
    {
#if SUWP_DEBUG
        std::cout << "WIFI start..." << std::endl;
#endif
    }
    bool *stationModeEnabled = (bool *)event_data;
    if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (*stationModeEnabled == true)
        {
#if SUWP_DEBUG
            std::cout << "WiFi connection failed, attempting to reconnect!" << std::endl;
#endif
            esp_wifi_connect();
        }
    }
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
#if SUWP_DEBUG
        std::cout << "Station connected!" << std::endl;
#endif
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
#if SUWP_DEBUG
        std::cout << "Station disconnected!" << std::endl;
#endif
    }
}

suwp_wifi::suwp_wifi()
{
#if SUWP_DEBUG
    std::cout << "Initialize wifi..." << std::endl;
#endif
    wifi_init();
}

suwp_wifi::~suwp_wifi()
{
    if (apModeEnabled == true)
    {
        wifi_ap_stop();
    }
    if (stationModeEnabled == true)
    {
        wifi_station_stop();
    }
    wifi_deinit();
}

suwp_err_t suwp_wifi::wifi_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        &stationModeEnabled,
                                                        NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    return SUWP_OK;
}

suwp_err_t suwp_wifi::wifi_deinit(void)
{
#if SUWP_DEBUG
    std::cout << "Wifi deinit!" << std::endl;
#endif
    ESP_ERROR_CHECK(esp_wifi_deinit());
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, NULL);
    // esp_netif_deinit(); // This function is currently not supported
    return SUWP_OK;
}

void suwp_wifi::wifi_set_ap_ssid(const char *newssid)
{
    strncpy(ap_ssid, newssid, sizeof(ap_ssid) - 1);
    ap_ssid[sizeof(ap_ssid) - 1] = '\0';
#if SUWP_DEBUG
    std::cout << "New wifi ap ssid: " << ap_ssid << std::endl;
#endif
}

void suwp_wifi::wifi_set_ap_password(const char *newpassword)
{
    strncpy(ap_password, newpassword, sizeof(ap_password) - 1);
    ap_password[sizeof(ap_password) - 1] = '\0';
#if SUWP_DEBUG
    std::cout << "New wifi ap password: " << ap_password << std::endl;
#endif
}

void suwp_wifi::wifi_ap_start(void)
{
    if (apModeEnabled == true && stationModeEnabled == true)
    {
    }
    else if (stationModeEnabled == true)
    {
        wifi_station_stop();
    }
    else if (apModeEnabled == true)
    {
        return;
    }
    wifi_ap_netif = esp_netif_create_default_wifi_ap();
#if SUWP_DEBUG
    std::cout << "Creates WIFI AP..." << std::endl;
#endif
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.ap.ssid, ap_ssid, sizeof(ap_ssid) - 1);
    wifi_config.ap.ssid[sizeof(wifi_config.ap.ssid) - 1] = '\0';
    wifi_config.ap.ssid_len = strlen(ap_ssid);
    wifi_config.ap.channel = ap_channel;
    memcpy(wifi_config.ap.password, ap_password, sizeof(ap_password) - 1);
    wifi_config.ap.password[sizeof(wifi_config.ap.password) - 1] = '\0';
    wifi_config.ap.max_connection = ap_max_connection;
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
    wifi_config.ap.authmode = WIFI_AUTH_WPA3_PSK;
    wifi_config.ap.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
#else
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
#endif
    wifi_config.ap.pmf_cfg.required = true;
    if (strlen(ap_password) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
#if SUWP_DEBUG
    std::cout << "WIFI ap start..." << std::endl;
#endif
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    apModeEnabled = true;
}

void suwp_wifi::wifi_ap_stop(void)
{
#if SUWP_DEBUG
    std::cout << "WIFI stop!" << std::endl;
#endif
    apModeEnabled = false;
    ESP_ERROR_CHECK(esp_wifi_stop());
    esp_netif_destroy_default_wifi(wifi_ap_netif);
}

void suwp_wifi::wifi_set_station_ssid(const char *newssid)
{
    strncpy(station_ssid, newssid, sizeof(station_ssid) - 1);
    station_ssid[sizeof(station_ssid) - 1] = '\0';
#if SUWP_DEBUG
    std::cout << "New wifi station ssid: " << station_ssid << std::endl;
#endif
}
void suwp_wifi::wifi_set_station_password(const char *newpassword)
{
    strncpy(station_password, newpassword, sizeof(station_password) - 1);
    station_password[sizeof(station_password) - 1] = '\0';
#if SUWP_DEBUG
    std::cout << "New wifi station password: " << station_password << std::endl;
#endif
}

void suwp_wifi::wifi_station_start(void)
{
    if (apModeEnabled == true && stationModeEnabled == true)
    {
    }
    else if (apModeEnabled == true)
    {
        wifi_ap_stop();
    }
    else if (stationModeEnabled == true)
    {
        return;
    }
    wifi_sta_netif = esp_netif_create_default_wifi_sta();
#if SUWP_DEBUG
    std::cout << "Creates WIFI STA..." << std::endl;
#endif
    esp_netif_set_hostname(wifi_sta_netif, "SHIERU");

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, station_ssid, sizeof(station_ssid) - 1);
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
    memcpy(wifi_config.sta.password, station_password, sizeof(station_password) - 1);
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';
#if SUWP_DEBUG
    std::cout << "WIFI config ssid: " << wifi_config.sta.ssid << std::endl;
    std::cout << "WIFI config password: " << wifi_config.sta.password << std::endl;
#endif
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
#if SUWP_DEBUG
    std::cout << "WIFI station start..." << std::endl;
#endif
    ESP_ERROR_CHECK(esp_wifi_start());
    stationModeEnabled = true;
#if SUWP_DEBUG
    std::cout << "Connect WiFi station to the AP..." << std::endl;
#endif
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void suwp_wifi::wifi_station_stop(void)
{
#if SUWP_DEBUG
    std::cout << "WIFI stop!" << std::endl;
#endif
    stationModeEnabled = false;
    ESP_ERROR_CHECK(esp_wifi_stop());
    esp_netif_destroy_default_wifi(wifi_sta_netif);
}