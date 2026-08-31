#include "mc_uart.h"
#include "MC_Protocol.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"



#define TAG "MC_UART"

#define MC_UART_PORT       UART_NUM_1
#define MC_UART_BAUDRATE   115200
#define MC_UART_BUF_SIZE   1024

static void MC_UART_Task(void *arg)
{
    uint8_t data[MC_UART_BUF_SIZE];

    while (1)
    {
        int len = uart_read_bytes(MC_UART_PORT,
                                  data,
                                  sizeof(data),
                                  pdMS_TO_TICKS(100));

        if (len > 0)
        {
            //ESP_LOGI(TAG, "uart收到 %d 字节数据", len);
            //ESP_LOG_BUFFER_HEX(TAG, data, len);

            MC_Stream_Parse(data, len);
        }
    }
}

void MC_UART_Init(void)
{
    uart_config_t config =
    {
        .baud_rate = MC_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(MC_UART_PORT,
                        MC_UART_BUF_SIZE,
                        0,
                        0,
                        NULL,
                        0);

    uart_param_config(MC_UART_PORT,
                      &config);

    uart_set_pin(MC_UART_PORT,
                 GPIO_NUM_17,
                 GPIO_NUM_18,
                 UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE);
    gpio_set_pull_mode(
    GPIO_NUM_18,
    GPIO_PULLUP_ONLY
    );
    
    xTaskCreate(MC_UART_Task,
                "mc_uart",
                4096,
                NULL,
                5,
                NULL);
}