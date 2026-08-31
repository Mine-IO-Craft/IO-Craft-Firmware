#ifndef __MC_WIFI_H__
#define __MC_WIFI_H__

#include <stdint.h>



typedef struct
{
    char ssid[32];
    char password[64];
} MC_Wifi_Config_t;

extern MC_Wifi_Config_t wifi_config;


extern bool wifi_connected;

void MC_Wifi_Init(void);

void wifi_save_config(
        const char *ssid,
        const char *password);




#endif

