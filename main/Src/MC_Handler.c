#include <stdint.h>
#include <stddef.h>
#include "MC_Protocol.h"
#include "esp_log.h"
#include "led_strip.h"

#include "MC_Task.h"

static const char *TAG = "MC_Handler";
#define BLINK_GPIO 48 // LED 连接的 GPIO 引脚

static led_strip_handle_t led_strip; // LED 灯带句柄
static void configure_led(void);


/*屎山代码警告*/
/*本文件虽然是Handler，但实际上其实负责协议-任务的转换*/
/*它负责把数据段里的data分给函数*/
/*说白了这里才是Task*/
/*但是懒得改了*/




/*--------------------------------------------------*/
/*--------------------------------------------------*/
/*--------------------------------------------------*/
//System_Version_Handler 0x00 0x00 系统版本
void System_Version_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG, "System Version 0.10");
}

//System_Reset_Handler 0x00 0x01 重启设备
void System_Reset_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG, "System Reset Command Received");
    //在这里添加重启系统的代码
    //例如: system_reset();
}

//MC_CMD_SYSTEM_DEFAULT 0x00 0x02 恢复出厂设置
void System_Default_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG, "System Default Command Received");
    //在这里添加恢复出厂设置的代码
    //例如: system_default();
}

//MC_CMD_SYSTEM_STRBLE 0x00 0x03 开启蓝牙 !!开启蓝牙会导致wifi关闭
void System_StrBle_Handler(MC_Packet_Format_t *pkt)
{

}

//MC_CMD_SYSTEM_STRWIFI 0x00 0x04 开启wifi !!开启wifi会导致蓝牙关闭
void System_StrWifi_Handler(MC_Packet_Format_t *pkt)
{

}

/*--------------------------------------------------*/
/*--------------------------------------------------*/
/*--------------------------------------------------*/
//MC_CMD_WS2812_SET_NUM 0x01 0x00
void WS2812_Set_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG, "WS2812 Set Command Received");
    MC_Task_WS2812_Init(pkt->data[0]);
    //在这里添加设置WS2812灯带的代码
    //例如: ws2812_set(pkt->data, pkt->length);
}

//MC_CMD_WS2812_SET_RGB 0x01 0x01
void WS2812_RGB_Handler(MC_Packet_Format_t *pkt)
{
    /*数据长度检查*/
    if(pkt->length != 4)
    {
        ESP_LOGE(TAG,"WS2812 packet length error:%d",pkt->length);
        return;
    }

    ESP_LOGI(TAG, "WS2812 RGB Command Received");
    MC_Task_WS2812_SetRGB(pkt->data[0],pkt->data[1], pkt->data[2], pkt->data[3]);
    //在这里添加设置WS2812灯带RGB值的代码
    //例如: ws2812_set_rgb(pkt->data, pkt->length);
}
/*--------------------------------------------------*/
/*--------------------------------------------------*/
/*--------------------------------------------------*/
//MC_CMD_MCDATA 0x02 0x00 生命值
void MC_Health_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"生命 = %u",pkt->data[0]);
    //在这里添加处理MC数据的代码
    //例如: mc_health_process(pkt->data, pkt->length);
}

//MC_CMD_MCDATA_FOOD 0x02 0x01 饱食度
void MC_Food_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"饱食 = %u",pkt->data[0]);
    //在这里添加处理MC饱食度的代码
    //例如: mc_food_process(pkt->data, pkt->length);
}

//MC_CMD_MCDATA_SATURATION 0x02 0x02 饱和度
void MC_Saturation_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI( TAG,"饱和 = %u",pkt->data[0]);
    //在这里添加处理MC饱和度的代码
    //例如: mc_saturation_process(pkt->data, pkt->length);
}

//MC_CMD_MCDATA_XYZ 0x02 0x03 坐标
void MC_Coordinate_Handler(MC_Packet_Format_t *pkt)
{
    int32_t x, y, z;

    memcpy(&x, &pkt->data[0], 4);
    memcpy(&y, &pkt->data[4], 4);
    memcpy(&z, &pkt->data[8], 4);

    ESP_LOGI(TAG,
             "X=%ld Y=%ld Z=%ld",
             (long)x,
             (long)y,
             (long)z);
}

//MC_CMD_MCDATA_BIOME 0x02 0x04 生物群系
void MC_BIOME_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"生物群系未实装");
}

//MC_CMD_MCDATA_LEVEL 0x02 0x05 等级
void MC_LEVEL_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"等级 = %u",pkt->data[0]);
}

//MC_CMD_MCDATA_DIFFICULTY 0x02 0x06 难度
void MC_DIFFICULTY_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"难度 = %u",pkt->data[0]);
}

//MC_CMD_MCDATA_TIME 0x02 0x07 时间
void MC_TIME_Handler(MC_Packet_Format_t *pkt)
{
     ESP_LOGI(TAG,"时间 = %u",pkt->data[0]);
}

//MC_CMD_MCDATA_WEATHER 0x02 0x08 天气
void MC_WEATHER_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"天气 = %u",pkt->data[0]);
}

//MC_CMD_MCDATA_SPEED 0x02 0x09 速度
void MC_SPEED_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"速度 = %u",pkt->data[0]);
}

//MC_CMD_MCDATA_DAY 0x02 0x0A 生存天数
void MC_DAY_Handler(MC_Packet_Format_t *pkt)
{
    ESP_LOGI(TAG,"生存天数 = %u",pkt->data[0]);
}

//MC_CMD_MCDATA_EXP 0x02 0x0B 经验值
void MC_EXP_Handler(MC_Packet_Format_t *pkt)
{
     ESP_LOGI(TAG,"经验值 = %u",pkt->data[0]);
}



/*--------------------------------------------------*/
/*--------------------------------------------------*/
/*--------------------------------------------------*/
//MC_CMD_GPIO_OUT 0x10 0x00
void MC_GPIO_OUT_Handler()
{
void MC_GPIO_OUT_Handler(MC_Packet_Format_t *pkt)
{
    uint8_t gpio_num;
    bool output_level;
    bool open_drain;
    bool pull_up;

    ESP_LOGI(TAG,"MC GPIO OUT Command Received");

    /*数据长度检查*/
    if(pkt->length != 4)
    {
        ESP_LOGE(TAG,"GPIO packet length error:%d",pkt->length);
        return;
    }

    /*解析数据*/
    gpio_num = pkt->data[0];
    output_level = (pkt->data[1] != 0);
    open_drain = (pkt->data[2] != 0);
    pull_up = (pkt->data[3] != 0);

    ESP_LOGI(TAG,
        "GPIO:%d LEVEL:%d MODE:%s PULL:%s",
        gpio_num,output_level,
        open_drain ? "OD" : "PP",
        pull_up ? "UP" : "DOWN"
    );

    /*执行任务*/
    MC_Task_GPIO_Output_Set(
        gpio_num,
        output_level,
        open_drain,
        pull_up
    );
    }
}
//MC_CMD_GPIO_IN 0x10 0x01


//MC_CMD_GPIO_PWM 0x10 0x02
void MC_GPIO_PWM_Handler(MC_Packet_Format_t *pkt)
{
    uint8_t gpio;
    uint8_t timer;
    uint8_t channel;
    uint16_t freq;
    uint16_t duty;
    ESP_LOGI(TAG,"MC GPIO PWM Command Received");
     /* 长度检查 */
    if(pkt->length != 7)
    {
        ESP_LOGE(TAG,"PWM packet length error:%d",pkt->length);
        return;
    }

    //提取数据 
    gpio = pkt->data[0];
    timer = pkt->data[1];
    channel = pkt->data[2];
    //大端模式
    freq = (pkt->data[3] << 8) | pkt->data[4];
    duty = (pkt->data[5] << 8) | pkt->data[6];

    ESP_LOGI(TAG,"GPIO:%d TIMER:%d CH:%d FREQ:%d DUTY:%d",gpio,timer,channel,freq,duty);
    MC_Task_PWM_Set(gpio,timer,channel,freq,duty);
}