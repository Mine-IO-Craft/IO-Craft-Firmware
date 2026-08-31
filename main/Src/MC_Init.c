#include <stdint.h>
#include <stddef.h>
#include "MC_Protocol.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_strip.h"
#include "esp_log.h"

#include "MC_Task.h"

/*-------------------------------*/
/*这个.C放一些初始化*/
/*-------------------------------*/


/*0x01 WS2812 初始化 *///实际测了一下,没有使用 0x01 0x00 初始化 就设置颜色会导致奔溃重启
//不知道这个提前初始化是不是一个好点子
void MC_WS2812_Init(void)
{
    // LED 灯带通用配置
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,                                // 设置 GPIO 引脚
        .max_leds = 1,                                               // 设置 LED 数量
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB, // 设置颜色格式
    };

    // RMT 后端特定配置
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // RMT 分辨率，10MHz
        .flags.with_dma = false,           // 禁用 DMA
    };
}

/*0x10 GPIO初始化*/

/*介于PWM的初始化复杂，所以在这里要写一个PWM频率和PWM占空比修改的函数*/
//选择定时器和设置频率
void PWM_Freq_Config(uint8_t timer,uint16_t freq)
{

    ledc_set_freq(
        LEDC_LOW_SPEED_MODE,
        timer,
        freq
    );

}

//选择通道和占空比
void PWM_Duty_Config(uint8_t channel,uint16_t duty)
{

    /*
        这里假设协议:
        duty:
        0~1000
        转换成LEDC占空比
    */
    uint32_t ledc_duty;
    ledc_duty =duty * 8191 / 1000;
    ledc_set_duty(
        LEDC_LOW_SPEED_MODE,
        channel,
        ledc_duty
    );
    ledc_update_duty(
        LEDC_LOW_SPEED_MODE,
        channel
    );
}

//初始化PWM
void PWM_Init(
    uint8_t gpio,
    uint8_t timer,
    uint8_t channel,
    uint16_t freq
)
{
    /*-------------------
        配置PWM Timer
        Timer负责：
        频率
        分辨率
    ---------------------*/

    ledc_timer_config_t timer_conf =
    {
        .speed_mode =
            LEDC_LOW_SPEED_MODE,
        .timer_num =
            timer,
        .duty_resolution =
            LEDC_TIMER_13_BIT,
        .freq_hz =
            freq,
        .clk_cfg =
            LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    /*-----------------------
        配置PWM Channel
        Channel负责：
        GPIO
        duty
    ------------------------*/

    ledc_channel_config_t channel_conf =
    {
        .gpio_num =
            gpio,
        .speed_mode =
            LEDC_LOW_SPEED_MODE,
        .channel =
            channel,
        .timer_sel =
            timer,
        .duty =
            0,
        .hpoint =
            0,
    };

    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    ESP_LOGI(
        "PWM",
        "PWM Init GPIO:%d TIMER:%d CH:%d FREQ:%d",
        gpio,
        timer,
        channel,
        freq
    );

}