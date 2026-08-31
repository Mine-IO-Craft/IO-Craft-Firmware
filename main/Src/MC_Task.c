#include <stdint.h>
#include <stddef.h>
#include "MC_Protocol.h"
#include "esp_log.h"
#include "led_strip.h"
#include "driver/gpio.h"

#include "MC_Init.h"
#include "MC_Task.h"
#include "MC_Protocol.h"

static const char *TAG = "MC_Task";
/*屎山代码警告*/
/*本文件虽然是Task，但实际上其实负责执行任务*/
/*它负责真正执行硬件操作*/
/*说白了这里才是Handler*/
/*但是懒得改了*/

/*-------------SYSTEM----------------*/
/*-------------SYSTEM----------------*/
/*-------------SYSTEM----------------*/





/*-------------WS2812----------------*/
/*-------------WS2812----------------*/
/*-------------WS2812----------------*/
/* 0x01 0x00 WS2812灯珠数量设置+初始化*/

static led_strip_handle_t led_strip; // LED 灯带句柄
void MC_Task_WS2812_Init(uint8_t num)
{

    // LED 灯带通用配置
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,                                // 设置 GPIO 引脚
        .max_leds = num,                                               // 设置 LED 数量
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB, // 设置颜色格式
    };

    // RMT 后端特定配置
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // RMT 分辨率，10MHz
        .flags.with_dma = false,           // 禁用 DMA
    };

    // 创建 LED 灯带对象
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_clear(led_strip); // 初始状态下清空灯带
}


/* 0x01 0x01 WS2812灯珠RGB值设置 */
void MC_Task_WS2812_SetRGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    // 设置第一个 LED 的颜色
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, index, r, g, b));
    // 刷新 LED 灯带以显示更改
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}


/*-------------MCPACK----------------*/
/*-------------MCPACK----------------*/
/*-------------MCPACK----------------*/

// MC_Task_MCDATA_LEVEL 0x02 0x05 等级
void MC_Task_MCDATA_LEVEL(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 4)
        return;

    int32_t level;

    memcpy(&level,
           pkt->data,
           sizeof(level));

    ESP_LOGI(TAG,
             "MC 等级 = %ld",
             (long)level);
}

// MC_Task_MCDATA_DIFFICULTY 0x02 0x06 难度
void MC_Task_MCDATA_DIFFICULTY(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 4)
        return;

    int32_t difficulty;

    memcpy(&difficulty,
           pkt->data,
           sizeof(difficulty));

    ESP_LOGI(TAG,
             "MC Difficulty = %ld",
             (long)difficulty);
}

// MC_Task_MCDATA_TIME 0x02 0x07 时间
void MC_Task_MCDATA_TIME(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 8)
        return;

    int64_t time;

    memcpy(&time,
           pkt->data,
           sizeof(time));

    ESP_LOGI(TAG,
             "MC Time = %lld",
             (long long)time);
}

// MC_Task_MCDATA_WEATHER 0x02 0x08 天气
void MC_Task_MCDATA_WEATHER(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 4)
        return;

    int32_t weather;

    memcpy(&weather,
           pkt->data,
           sizeof(weather));

    ESP_LOGI(TAG,
             "MC Weather = %ld",
             (long)weather);
}

// MC_Task_MCDATA_SPEED 0x02 0x09 速度
void MC_Task_MCDATA_SPEED(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 4)
        return;

    float speed;

    memcpy(&speed,
           pkt->data,
           sizeof(speed));

    ESP_LOGI(TAG,
             "MC Speed = %.2f",
             speed);
}

// MC_Task_MCDATA_DAY 0x02 0x0A 生存天数
void MC_Task_MCDATA_DAY(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 4)
        return;

    int32_t day;

    memcpy(&day,
           pkt->data,
           sizeof(day));

    ESP_LOGI(TAG,
             "MC Day = %ld",
             (long)day);
}

// MC_Task_MCDATA_EXP 0x02 0x0B 经验值
void MC_Task_MCDATA_EXP(MC_Packet_Format_t *pkt)
{
    if(pkt->length < 4)
        return;

    int32_t exp;

    memcpy(&exp,
           pkt->data,
           sizeof(exp));

    ESP_LOGI(TAG,
             "MC Exp = %ld",
             (long)exp);
}


/*-------------GPIO----------------*/
/*-------------GPIO----------------*/
/*-------------GPIO----------------*/

/*0x10 0x00 GPIO输出设置*///输出电平+输出模式+是否开启上拉
void MC_Task_GPIO_Output_Set(
    uint8_t gpio_num,
    bool output_level,
    bool open_drain,
    bool pull_up
)
{

    bool need_init=false;
    if(!gpio_state[gpio_num].initialized)
    {
        need_init=true;
    }
    if(gpio_state[gpio_num].open_drain 
        != open_drain)
    {
        need_init=true;
    }
    if(gpio_state[gpio_num].pull_up
        != pull_up)
    {
        need_init=true;
    }
    if(need_init)
    {
        gpio_config_t io_conf={0};
        io_conf.pin_bit_mask =
            (1ULL << gpio_num);
        //推挽/开漏
        if(open_drain)
        {
            io_conf.mode =
                GPIO_MODE_OUTPUT_OD;
        }
        else
        {
            io_conf.mode =
                GPIO_MODE_OUTPUT;
        }
        //上拉/下拉
        if(pull_up)
        {
            io_conf.pull_up_en =
                GPIO_PULLUP_ENABLE;
        }
        else
        {
            io_conf.pull_up_en =
                GPIO_PULLUP_DISABLE;
        }

        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.intr_type    = GPIO_INTR_DISABLE;
        gpio_config(&io_conf);

        gpio_state[gpio_num].initialized = true;
        gpio_state[gpio_num].open_drain  = open_drain;
        gpio_state[gpio_num].pull_up     = pull_up;
    }
    //设置输出电平
    gpio_set_level(gpio_num,output_level);
    gpio_state[gpio_num].level = output_level;

}


/*0x10 0x01 GPIO 输入设置*/


/*0x10 0x02 PWM控制*///PWM配置在MC_Init.c
void MC_Task_PWM_Set(
    uint8_t gpio,
    uint8_t timer,
    uint8_t channel,
    uint16_t freq,
    uint16_t duty
)
{
    MC_PWM_State_t *pwm = &pwm_state[channel];

    /*---------------
    第一次使用，且CH分配的IO和timer有变化，那么重新初始化
    ----------------*/
    if(!pwm->initialized || pwm->gpio != gpio || pwm->timer != timer)
    {
        PWM_Init(
            gpio,
            timer,
            channel,
            freq
        );
        PWM_Duty_Config(channel,duty);
        pwm->initialized=true;
        pwm->freq_hz=freq;
        pwm->duty_cycle=duty;
        pwm->gpio=gpio;
        pwm->timer=timer;
        pwm->channel=channel;
       
        return;
    }

    /*
        频率变化
    */
    if(pwm->freq_hz != freq)
    {
        PWM_Freq_Config(timer,freq);
        pwm->freq_hz=freq;
    }
    /*
        占空比变化
    */
    if(pwm->duty_cycle != duty)
    {
        PWM_Duty_Config(channel,duty);
        pwm->duty_cycle=duty;
    }
}
