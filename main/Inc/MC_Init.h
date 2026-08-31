#ifndef MC_INIT_H
#define MC_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void MC_WS2812_Init(void);
//void MC_GPIO_Init(void);


void PWM_Freq_Config(
    uint8_t timer,
    uint16_t freq
);

void PWM_Duty_Config(
    uint8_t channel,
    uint16_t duty
);

void PWM_Init(
    uint8_t gpio,
    uint8_t timer,
    uint8_t channel,
    uint16_t freq
);



#ifdef __cplusplus
}
#endif

#endif // MC_INIT_H