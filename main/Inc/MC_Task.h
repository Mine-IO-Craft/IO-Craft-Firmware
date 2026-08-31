#ifndef __MC_TASK_H__
#define __MC_TASK_H__

#define BLINK_GPIO 48 // LED 连接的 GPIO 引脚

//包含用于修改系统信息

//包含修改WS2812灯带的模块
void MC_Task_WS2812_Init(uint8_t num);
void MC_Task_WS2812_SetRGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
//0x02  包含MC数据的模块
void MC_Task_MCDATA_LEVEL(MC_Packet_Format_t *pkt);
void MC_Task_MCDATA_DIFFICULTY(MC_Packet_Format_t *pkt);
void MC_Task_MCDATA_TIME(MC_Packet_Format_t *pkt);
void MC_Task_MCDATA_WEATHER(MC_Packet_Format_t *pkt);
void MC_Task_MCDATA_SPEED(MC_Packet_Format_t *pkt);
void MC_Task_MCDATA_DAY(MC_Packet_Format_t *pkt);
void MC_Task_MCDATA_EXP(MC_Packet_Format_t *pkt);
//包含用于设置GPIO的输入输出状态
void MC_Task_GPIO_Output_Set(uint8_t gpio_num,bool output_level,bool open_drain,bool pull_up);
void MC_Task_PWM_Set(
    uint8_t gpio,
    uint8_t timer,
    uint8_t channel,
    uint16_t freq,
    uint16_t duty
);


//GPIO 状态表，执行task时判断有无初始化，避免重复初始化
typedef struct
{
    bool initialized;   //区分是否是第一次配置
    bool open_drain;    //推挽/开漏
    bool pull_up;       //上拉/下拉 没有浮空输入
    bool level;         //高低电平
}MC_GPIO_State_t;
//给48个io分配一个状态表
static MC_GPIO_State_t gpio_state[48];

//PWM的状态表
typedef struct
{
    bool initialized;
    uint16_t freq_hz;
    uint16_t duty_cycle;
    uint8_t gpio;
    uint8_t channel;
    uint8_t timer;
}MC_PWM_State_t;

//一共有8个CH，这里给每个CH一个状态表，不给io了，毕竟输出PWM的是CH
static MC_PWM_State_t pwm_state[8];

#endif // __MC_TASK_H__
