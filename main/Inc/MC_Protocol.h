#ifndef __MC_PROTOCOL_H__
#define __MC_PROTOCOL_H__

#include <stdint.h>

/* 一个BLE数据包最大244Byte左右，这里预留240Byte数据区 */
#define MC_PACKET_DATA_SIZE    244
#define MCL            3    //module + command + length 的长度

#define MODULE_MAX  32
#define CMD_MAX     32






extern void MC_Handler_Init(void);
extern void MC_Stream_Parse(uint8_t *buf, uint16_t len);



/* 模块编号枚举 */
typedef enum
{
    MC_MODULE_SYSTEM = 0x00,    //包含用于修改系统信息
    MC_MODULE_WS2812 = 0x01,    //包含修改WS2812灯带的模块
    MC_MODULE_MCDATA = 0x02,    //包含MC数据的模块
    MC_MODULE_SENSOR = 0x03,    //包含用于设置传感器模块
    MC_MODULE_MOTOR  = 0x04,    //包含电机运动相关模块(占个茅坑)
    MC_MODULE_DISPLAY= 0x05,    //包含用于设置显示屏的模块
    MC_MODULE_AUDIO  = 0x06,    //包含用于设置音频的模块(用得着吗?先占个茅坑)
    MC_MODULE_GPIO   = 0x10,    //包含用于设置GPIO的输入输出状态(简单控制高|低|推挽|开漏)
    MC_MODULE_MAX    = 0xFF     //好像也没什么用,反正规定了最大单位(真的能用完255个模块吗?)
}MC_Module_t;


/* 数据包格式结构体 */
typedef struct
{
    uint8_t  module;                    //模块号
    uint8_t  command;                   //命令号
    uint8_t length;                   //数据长度
    uint8_t  data[MC_PACKET_DATA_SIZE]; //数据区
}MC_Packet_Format_t;


/*0x00 系统信息结构体 */
typedef struct
{
    int Version;    //获取版本信息
    int Reset;      //重启系统
    int Default;    //恢复出厂设置
}MC_System_t;

typedef enum
{
    MC_CMD_SYSTEM_VERSION = 0x00,  //获取版本信息
    MC_CMD_SYSTEM_RESET   = 0x01,  //重启系统
    MC_CMD_SYSTEM_DEFAULT = 0x02,  //恢复出厂设置
    MC_CMD_SYSTEM_STRBLE  = 0x03,  //开启BLE
    MC_CMD_SYSTEM_STRWIFI = 0x04,  //开启Wifi
}MC_System_Cmd_t;


/*0x01 WS2812结构体 *///这里还没想好要怎么设置多个WS2812灯带
typedef struct
{
    int Num;        //设置WS2812灯带数量
    int Set_RGB;    //设置WS2812红色值
}MC_WS2812_t;

typedef enum
{
    MC_CMD_WS2812_SET_NUM = 0x00,  //设置WS2812灯带数量
    MC_CMD_WS2812_SET_RGB = 0x01,  //设置WS2812RGB值
}MC_WS2812_Cmd_t;


/*0x02 MC数据结构体 */
typedef struct 
{
    float health;       //获取血量信息
    int food;           //获取饱食度信息
    float saturation;   //获取饱和度信息
    int x, y, z;        //获取坐标信息
    int biome;          //获取生物群系信息
    int level;          //获取等级信息
    int exp;            //获取经验值信息
    int difficulty;     //获取难度信息
    int time;           //获取时间信息
    int weather;        //获取天气信息
    float speed;        //获取速度信息
    int day;            //生存天数
}MC_Data_t;

typedef enum
{
    MC_CMD_MCDATA_HEALTH = 0x00,     //获取血量信息
    MC_CMD_MCDATA_FOOD = 0x01,       //获取饱食度信息
    MC_CMD_MCDATA_SATURATION = 0x02, //获取饱和度信息
    MC_CMD_MCDATA_COORDINATE = 0x03, //获取坐标信息
    MC_CMD_MCDATA_BIOME = 0x04,      //获取生物群系信息
    MC_CMD_MCDATA_LEVEL = 0x05,      //获取等级信息
    MC_CMD_MCDATA_DIFFICULTY = 0x06,  //获取难度信息
    MC_CMD_MCDATA_TIME = 0x07,       //获取时间信息
    MC_CMD_MCDATA_WEATHER = 0x08,    //获取天气信息
    MC_CMD_MCDATA_SPEED = 0x09,      //获取速度信息
    MC_CMD_MCDATA_DAY = 0x0A,        //生存天数
    MC_CMD_MCDATA_EXP = 0x0B,        //获取经验值信息//not use
}MC_Data_Cmd_t;


/*0x03 传感器结构体 *///这tm真的会被用到吗
typedef struct
{
    int Temperature;  //获取温度信息
    int Humidity;     //获取湿度信息
    int Pressure;     //获取气压信息
}MC_Sensor_t;

typedef enum
{
    MC_CMD_SENSOR_TEMPERATURE = 0x00, //获取温度信息
    MC_CMD_SENSOR_HUMIDITY = 0x01,    //获取湿度信息
    MC_CMD_SENSOR_PRESSURE = 0x02     //获取气压信息
}MC_Sensor_Cmd_t;


/*0x04 电机结构体 */
typedef struct
{
    int Speed;      //设置电机速度
    int Dir;        //设置电机角度
}MC_Motor_t;

typedef enum
{
    MC_CMD_MOTOR_SPEED = 0x00, //设置电机速度
    MC_CMD_MOTOR_DIR = 0x01    //设置电机角度
}MC_Motor_Cmd_t;


/*0x05 显示屏结构体 */
typedef struct
{
    int Brightness; //设置显示屏亮度
    int Contrast;   //设置显示屏对比度
}MC_Display_t;

typedef enum
{
    MC_CMD_DISPLAY_BRIGHTNESS = 0x00, //设置显示屏亮度
    MC_CMD_DISPLAY_CONTRAST = 0x01    //设置显示屏对比度
}MC_Display_Cmd_t;


/*0x06 音频结构体 */
typedef struct
{
    int Volume;     //设置音频音量
    int Frequency;  //设置音频频率
}MC_Audio_t;

typedef enum
{
    MC_CMD_AUDIO_VOLUME = 0x00,    //设置音频音量
    MC_CMD_AUDIO_FREQUENCY = 0x01  //设置音频频率
}MC_Audio_Cmd_t;


/*0x10 GPIO结构体 *///这里其实应该和下面的enum一样，但是用不着 懒得改了
typedef struct
{
    int PP_H;      //设置GPIO为推挽输出
    int OD_H;      //设置GPIO为开漏输出
    int PP_L;      //设置GPIO为推挽输出
    int OD_L;      //设置GPIO为开漏输出
    int OFF;       //设置GPIO为输入模式
}MC_GPIO_t;

typedef enum
{
    MC_CMD_GPIO_OUT_SET  = 0x00,   //GPIO输出设置
    MC_CMD_GPIO_IN_SET   = 0x01,   //GPIO输入设置
    MC_CMD_GPIO_PWM      = 0x02    //GPIO PWM设置
}MC_GPIO_Cmd_t;

#endif
