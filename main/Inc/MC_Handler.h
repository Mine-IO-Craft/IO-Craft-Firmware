#ifndef MC_HANDLER_H
#define MC_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void System_Version_Handler(MC_Packet_Format_t *pkt);
void System_Reset_Handler(MC_Packet_Format_t *pkt);
void System_Default_Handler(MC_Packet_Format_t *pkt);
void System_StrBle_Handler(MC_Packet_Format_t *pkt);
void System_StrWifi_Handler(MC_Packet_Format_t *pkt);


void WS2812_Set_Handler(MC_Packet_Format_t *pkt);
void WS2812_RGB_Handler(MC_Packet_Format_t *pkt);

void MC_Health_Handler(MC_Packet_Format_t *pkt);
void MC_Food_Handler(MC_Packet_Format_t *pkt);
void MC_Saturation_Handler(MC_Packet_Format_t *pkt);
void MC_Coordinate_Handler(MC_Packet_Format_t *pkt);
void MC_BIOME_Handler(MC_Packet_Format_t *pkt);
void MC_LEVEL_Handler(MC_Packet_Format_t *pkt);
void MC_DIFFICULTY_Handler(MC_Packet_Format_t *pkt);
void MC_TIME_Handler(MC_Packet_Format_t *pkt);
void MC_WEATHER_Handler(MC_Packet_Format_t *pkt);
void MC_SPEED_Handler(MC_Packet_Format_t *pkt);
void MC_DAY_Handler(MC_Packet_Format_t *pkt);
void MC_EXP_Handler(MC_Packet_Format_t *pkt);

void MC_GPIO_OUT_Handler(MC_Packet_Format_t *pkt);
void MC_GPIO_PWM_Handler(MC_Packet_Format_t *pkt);

#ifdef __cplusplus
}
#endif

#endif // MC_HANDLER_H
