#ifndef MC_TCP_H
#define MC_TCP_H


#include "esp_netif_ip_addr.h"

extern esp_ip4_addr_t device_ip;



//启动TCP服务器
void MC_TCP_Server_Start(void);


#endif
