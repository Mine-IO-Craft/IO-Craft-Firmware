#include "MC_Wifi.h"
#include "wifi_ap.h"
#include "wifi_sta.h"
#include "web_server.h"


#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"


static const char *TAG = "MC_Wifi";
MC_Wifi_Config_t wifi_config;



static void wifi_nvs_init(void);
static bool wifi_load_config(MC_Wifi_Config_t *config);
static bool wifi_scan_match(char *ssid);
static bool wifi_config_valid = false;

bool wifi_wait_connect(void);
bool wifi_connected = false; //用于判断有没有超时


void MC_Wifi_Init(void)
{
    ESP_LOGI(TAG,
             "WiFi Init");
    ESP_LOGI(TAG,"WiFi Init");
    // TCP/IP协议栈
    ESP_ERROR_CHECK(
        esp_netif_init()
    );
    // 默认事件循环
    ESP_ERROR_CHECK(
        esp_event_loop_create_default()
    );
    // WiFi驱动初始化
    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(
        esp_wifi_init(&cfg)
    );
    // 创建STA接口
    esp_netif_create_default_wifi_sta();
    // 创建AP接口
    esp_netif_create_default_wifi_ap();
    // 注册事件
    wifi_sta_init();

    /*
        1. 初始化NVS
    */
    wifi_nvs_init();
    /*
        2. 读取保存的WiFi
    */
    if(wifi_load_config(&wifi_config))
    {
        //开启sta
        esp_wifi_start();
        ESP_LOGI(TAG,
                 "Found WiFi config: %s",
                 wifi_config.ssid);
        
        //连接保存的SSID&PassWord
        wifi_sta_connect(
        wifi_config.ssid,
        wifi_config.password,
        false);
         //等待连接结果,超时跳到AP
        if(wifi_wait_connect())
        {
            ESP_LOGI(TAG,
                 "WiFi connected");
        }
        else
        {
            ESP_LOGW(TAG,
                    "WiFi connect timeout");
            wifi_ap_init();
            web_server_start();
        }
    }

    else
    {
        ESP_LOGI(TAG,
                 "No WiFi config");
        /*
            第一次使用
            开AP配网
        */
        wifi_ap_init();
        web_server_start();
    }
}


//初始化NVS
static void wifi_nvs_init(void)
{
    esp_err_t ret;
    ret = nvs_flash_init();

    if(ret == ESP_ERR_NVS_NO_FREE_PAGES ||
       ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}


//保存函数
void wifi_save_config(
        const char *ssid,
        const char *password)
{

    nvs_handle_t handle;
    ESP_ERROR_CHECK(
        nvs_open(
            "wifi",
            NVS_READWRITE,
            &handle
        )
    );
    nvs_set_str(
        handle,
        "ssid",
        ssid
    );
    nvs_set_str(
        handle,
        "password",
        password
    );
    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG,
             "WiFi config saved");

}


//读取函数
static bool wifi_load_config(
        MC_Wifi_Config_t *config)
{
    ESP_LOGI(TAG,
         "Load SSID:%s PASSWORD:%s",
         config->ssid,
         config->password);

    nvs_handle_t handle;
    esp_err_t ret;
    ret = nvs_open(
            "wifi",
            NVS_READONLY,
            &handle
    );
    if(ret != ESP_OK)
    {
        return false;
    }

    size_t ssid_len = sizeof(config->ssid);
    size_t pass_len = sizeof(config->password);

    ret = nvs_get_str(
            handle,
            "ssid",
            config->ssid,
            &ssid_len
    );
    if(ret != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    ret = nvs_get_str(
            handle,
            "password",
            config->password,
            &pass_len
    );
    nvs_close(handle);
    if(ret != ESP_OK)
    {
        return false;
    }
    return true;
}


// 扫描附近WiFi，目前被超时启动AP替代
static bool wifi_scan_match(char *ssid)
{
    // TODO:
    // 扫描附近WiFi

    return false;
}

//超时判断函数
bool wifi_wait_connect(void)
{
    int timeout = 10;
    while(timeout--)
    {
        if(wifi_connected)
        {
            return true;
        }
        vTaskDelay(
            pdMS_TO_TICKS(1000)
        );
    }
    return false;
}