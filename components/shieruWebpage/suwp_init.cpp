#include <iostream>
// 组件的头文件
#include "suwp_type.h"
#include "wifi.h"
#include "http_server.h"
// esp-idf的头文件
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"

void suwp_sys_init(void)
{
#if SUWP_DEBUG
    std::cout << "Initialize nvs..." << std::endl;
#endif
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}

void suwp_task(void *arg)
{
    std::cout << "Shieru task start!" << std::endl;
    std::cout << "Version: " << SUWP_VERSION << std::endl;
    suwp_sys_init();
// ===================================================
    suwp_wifi wifi;
    wifi.wifi_set_station_ssid("ESP32_DEV_KIT");
    wifi.wifi_set_station_password("24171572417157");
    wifi.wifi_station_start();
    suwp_http_server http_server;
// ===================================================
    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

extern "C" void suwp_init(void)
{
    std::cout << "Hello, Shieru!" << std::endl;
    xTaskCreate(suwp_task, "suwp_task", 1024 * 8, NULL, 13, NULL);
}
