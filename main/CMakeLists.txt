idf_component_register(SRCS 
                        "main.c"
                        "Src/MC_Ble.c"
                        "Src/MC_Protocol.c"
                        "Src/MC_Uart.c"
                        "Src/MC_Handler.c"
                        "Src/MC_Task.c"
                        "Src/MC_Init.c"
                        "Src/wifi_ap.c"
                        "Src/web_server.c"
                        "Src/wifi_sta.c"
                        "Src/MC_Wifi.c"
                        "Src/MC_TCP.c"
                       

                        PRIV_REQUIRES 
                        esp_driver_gpio
                        esp_driver_ledc
                        esp_driver_rmt
                        esp_driver_uart
                        led_strip
                        esp_wifi
                        nvs_flash
                        esp_http_server
                        bt
                        INCLUDE_DIRS
                        "."
                        "Inc")
