#include "MC_Protocol.h"
#include <string.h>
#include "esp_log.h"
#include "MC_Handler.h"


static const char *TAG = "MC_Protocol";



//handler 类型定义
typedef void (*mc_handler_t)(MC_Packet_Format_t *pkt);
//handler 表格
static mc_handler_t handler_table[MODULE_MAX][CMD_MAX];
/* 当前收到的数据包 */
static MC_Packet_Format_t Packet;
//打印要用的变量
const char *cmd_name = "Unknown";



//注册函数
static void mc_register_handler(uint8_t module, uint8_t cmd, mc_handler_t handler)
{
    handler_table[module][cmd] = handler;
    //测试用，没bug就注释
    //ESP_LOGI(TAG,"handler=%p",handler);
}

//注册函数处理事件
void MC_Handler_Init(void)
{
    mc_register_handler(MC_MODULE_SYSTEM,   MC_CMD_SYSTEM_VERSION,    System_Version_Handler);
    mc_register_handler(MC_MODULE_SYSTEM,   MC_CMD_SYSTEM_RESET,      System_Reset_Handler);
    mc_register_handler(MC_MODULE_SYSTEM,   MC_CMD_SYSTEM_DEFAULT,    System_Default_Handler);
    mc_register_handler(MC_MODULE_SYSTEM,   MC_CMD_SYSTEM_STRBLE,     System_StrBle_Handler);
    mc_register_handler(MC_MODULE_SYSTEM,   MC_CMD_SYSTEM_STRWIFI,    System_StrWifi_Handler);

    mc_register_handler(MC_MODULE_WS2812,   MC_CMD_WS2812_SET_NUM,    WS2812_Set_Handler);
    mc_register_handler(MC_MODULE_WS2812,   MC_CMD_WS2812_SET_RGB,    WS2812_RGB_Handler);

    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_HEALTH,     MC_Health_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_FOOD,       MC_Food_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_SATURATION, MC_Saturation_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_COORDINATE, MC_Coordinate_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_LEVEL,      MC_LEVEL_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_DIFFICULTY, MC_DIFFICULTY_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_TIME,       MC_TIME_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_WEATHER,    MC_WEATHER_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_SPEED,      MC_SPEED_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_DAY,        MC_DAY_Handler);
    mc_register_handler(MC_MODULE_MCDATA,   MC_CMD_MCDATA_EXP,        MC_EXP_Handler);
    
    mc_register_handler(MC_MODULE_GPIO,     MC_CMD_GPIO_OUT_SET,      MC_GPIO_OUT_Handler);
    mc_register_handler(MC_MODULE_GPIO,     MC_CMD_GPIO_PWM,          MC_GPIO_PWM_Handler);
}

//分发函数
void MC_Dispatch(MC_Packet_Format_t *pkt)
{
    //测试为什么PWMtask没反应
    ESP_LOGI(TAG,"Dispatch mod=%02X cmd=%02X",pkt->module,pkt->command);

    if(pkt->module >= MODULE_MAX || pkt->command >= CMD_MAX)
        return;

    mc_handler_t handler = handler_table[pkt->module][pkt->command];

    if(handler)
    {
        handler(pkt);
    }
    else
    {
        ESP_LOGW(TAG, "No handler: module=%d cmd=%d", pkt->module, pkt->command);
    }
}


//拆包+解析函数
void MC_Stream_Parse(uint8_t *buf, uint16_t len)
{
    uint16_t Pos = 0;       //当前解析位置
    uint8_t index = 0;
    while (Pos + MCL <= len)
    {
        //拆包
        MC_Packet_Format_t pkt;
        pkt.module = buf[Pos];
        pkt.command = buf[Pos + 1];
        pkt.length =buf[Pos + 2];
        /*ESP_LOGI(TAG,
                 "Packet[%d]: module=0x%02X cmd=0x%02X len=%d",
                 index,
                 pkt.module,
                 pkt.command,
                 pkt.length);*/

         //判断数据是否完整
        if(Pos + MCL + pkt.length > len)
        {
            ESP_LOGW(TAG,
                     "Data incomplete: need=%d remain=%d",
                     Pos + MCL + pkt.length,
                     len);
            break;
        }
        memcpy(pkt.data,
               &buf[Pos + MCL],
               pkt.length);
        // 分发
        MC_Dispatch(&pkt);
        index++;
        // 更新位置
        Pos += MCL + pkt.length;
    }
    //测试用
    //ESP_LOGI(TAG,"MC_Stream_Parse Finished: packets=%d bytes=%d",index,Pos);
    
}

