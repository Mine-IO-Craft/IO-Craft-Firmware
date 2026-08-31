#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "led_strip.h"

#include "wifi_ap.h"
#include "web_server.h"
#include "MC_Ble.h"
#include "MC_Uart.h"
#include "MC_Wifi.h"
#include "MC_TCP.h"

#include "MC_Protocol.h"
//#include "MC_Init.h"


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    //wifi_ap_init();
    //web_server_start();
    MC_UART_Init();     // 初始化 UART
    MC_BLE_Init();      // 初始化蓝牙
    //MC_WS2812_Init();   //初始化ws2812，实测有bug，搁置
    MC_Wifi_Init();     //Wifi的初始化，会判断有没有保存SSID，没有就开始AP配网
    MC_TCP_Server_Start();   //启动UDP和TCP服务器
    
    MC_Handler_Init();  // 初始化处理器

    /*while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }*/
}

