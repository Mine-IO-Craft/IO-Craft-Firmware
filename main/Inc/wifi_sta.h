#ifndef __WIFI_STA_H__
#define __WIFI_STA_H__

#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif_ip_addr.h"

void wifi_sta_init(void);

void wifi_sta_connect(const char *ssid,
                      const char *password,
                      bool save);

void wifi_event_handler(void *arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void *event_data);


//extern esp_ip4_addr_t device_ip;
                    

#endif
