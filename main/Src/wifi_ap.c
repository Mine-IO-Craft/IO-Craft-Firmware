#include "wifi_ap.h"

#include "esp_log.h"
#include "esp_event.h"

#include "nvs_flash.h"
#include "led_strip.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#define WIFI_AP_SSID      "Orange-AP"
#define WIFI_AP_PASSWORD  "12345678"
#define WIFI_AP_MAX_CONN  4

static const char *TAG = "WIFI_AP";  // Tag for ESP_LOG


// Event handler for WiFi events
static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
     /* WiFi事件 */
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_AP_STACONNECTED: //有设备接入AP
            {
                wifi_event_ap_staconnected_t *event =
                    (wifi_event_ap_staconnected_t *)event_data;

                ESP_LOGI(TAG,
                         "STA Connected, AID=%d",
                         event->aid);
                
                break;
            }

            case WIFI_EVENT_AP_STADISCONNECTED://有设备断开AP
            {
                wifi_event_ap_stadisconnected_t *event =
                    (wifi_event_ap_stadisconnected_t *)event_data;

                ESP_LOGI(TAG,
                         "STA Disconnected, AID=%d",
                         event->aid);
                break;
            }

            default:
                break;
        }
    }
     /* IP事件 */
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
            {
                ip_event_got_ip_t *event =
                    (ip_event_got_ip_t *)event_data;

                ESP_LOGI(TAG,
                         "STA Got IP: " IPSTR,
                         IP2STR(&event->ip_info.ip));

                /* 获取IP成功，关闭AP */
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

                break;
            }

            default:
                break;
        }
    }
}


void wifi_ap_init(void)
{

    wifi_config_t wifi_config =
    {
        .ap =
        {
            .ssid = WIFI_AP_SSID,
            .password = WIFI_AP_PASSWORD,
            .channel = 1,
            .max_connection = WIFI_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };


    ESP_ERROR_CHECK(
        esp_wifi_set_mode(WIFI_MODE_AP)
    );


    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_AP,
            &wifi_config
        )
    );


    ESP_ERROR_CHECK(
        esp_wifi_start()
    );

}