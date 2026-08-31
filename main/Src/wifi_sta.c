#include "wifi_sta.h"

#include <string.h>

#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs.h"
#include "MC_Wifi.h"
#include "MC_TCP.h"

//#include "esp_netif_ip_addr.h"

static const char *TAG = "WIFI_STA";
static bool wifi_need_save = false; //是否需要保存SSID和password


static char current_ssid[32];
static char current_password[64];






void wifi_sta_init(void)
{
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            NULL
        )
    );


    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL,
            NULL
        )
    );


    ESP_LOGI(TAG,"wifi sta event init ok");
}

/*
 * 连接指定WiFi
 *
 * 参数：
 *      ssid      WiFi名称
 *      password  WiFi密码
 */
void wifi_sta_connect(const char *ssid,
                      const char *password,
                      bool save)
{
    wifi_need_save = save;//给flag赋值
    wifi_config_t wifi_config = {0};
    ESP_LOGI(TAG,
         "SSID=[%s] PASS=[%s]",
         ssid,
         password);

    /* 保存当前连接信息 注意：这里不是写入NVS，只是临时保存 */
    strcpy(current_ssid, ssid);
    strcpy(current_password, password);

    /* 拷贝SSID */
    memcpy(wifi_config.sta.ssid,
           ssid,
           strlen(ssid));

    /* 拷贝密码 */
    memcpy(wifi_config.sta.password,
           password,
           strlen(password));

    ESP_LOGI(TAG,
             "Connecting to %s ...",
             ssid);

    /* 如果已经连接，先断开 */
    esp_wifi_disconnect();

    /* 切换到STA模式*/
    esp_wifi_set_mode(WIFI_MODE_STA);


    /* 设置STA配置 */
    esp_wifi_set_config(WIFI_IF_STA,
                        &wifi_config);


    /* 开始连接
       注意：
       这里只是发起连接请求
       结果要等事件通知 */
    esp_wifi_connect();
}


//把SSID和PassWord保存到NVS里,方便下次直接连接
/*static void wifi_save_config(const char *ssid,
                      const char *password)
{
    nvs_handle_t handle;
    // 打开NVS
    nvs_open("wifi",
             NVS_READWRITE,
             &handle);
    // 保存SSID
    nvs_set_str(handle,
                "ssid",
                ssid);
    // 保存PassWord   
    nvs_set_str(handle,
                "password",
                password);
    nvs_commit(handle);// 写入Flash
    nvs_close(handle);// 关闭
}*/


//回调函数，连接后自动执行
void wifi_event_handler(void *arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void *event_data)
{

    ESP_LOGI(TAG,
             "EVENT:%s id:%ld",
             event_base,
             event_id);


    if(event_base == IP_EVENT &&
       event_id == IP_EVENT_STA_GOT_IP)
    {

        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;


        ESP_LOGI(TAG,
                 "Got IP:" IPSTR,
                 IP2STR(&event->ip_info.ip));
        
        //保存IP地址,等广播时候用
        device_ip = event->ip_info.ip;

        wifi_connected = true;//超时判断函数

        if(wifi_need_save == true)
        {
        wifi_save_config(
            current_ssid,
            current_password
        );
        }
    }
}
