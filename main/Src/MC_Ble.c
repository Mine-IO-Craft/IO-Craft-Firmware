/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * BLE 功能参考 ESP-IDF bleprph 示例（Apache-2.0）编写：
 * https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/nimble/bleprph
 */

#include "esp_log.h"

#include "host/ble_hs.h"
#include "host/util/util.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "MC_Ble.h"
#include "MC_Protocol.h"

#define TAG "MC_Ble"
#define DEVICE_NAME "Orange_Ble"
#define SERVICE_UUID   0xFFF0
#define RX_UUID        0xFFF1
#define TX_UUID        0xFFF2

static uint16_t rx_val_handle;
static uint16_t tx_val_handle;
static bool ble_adv_active = false;

/* 接收到BLE数据 */
static void MC_BLE_RxCallback(uint8_t *data, uint16_t len)
{
    ESP_LOGI(TAG, "收到 %d 字节数据", len);
    /* 打印收到的数据 */
    ESP_LOG_BUFFER_HEX(TAG, data, len);
    /* 这里以后再解析协议 */
    MC_Stream_Parse(data, len);
}

/* 主机读取数据 */
static void MC_BLE_TxCallback(struct os_mbuf *om)
{
    const char *value = "Hello from ESP32";
    os_mbuf_append(om,
                   value,
                   strlen(value));
}

static int gatt_event_handler(uint16_t conn_handle,
                              uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt,
                              void *arg)
{
    switch(ctxt->op)
    {
        /* 主机-data->从机 */
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        {
            if(attr_handle == rx_val_handle)
            {
                uint8_t rx_buffer[244];
                uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
                os_mbuf_copydata(ctxt->om,
                                 0,
                                 len,
                                 rx_buffer);
                MC_BLE_RxCallback(rx_buffer,
                                  len);
            }
            break;
        }
        /* 从机-data->主机 */ 
        case BLE_GATT_ACCESS_OP_READ_CHR:
        {
            if(attr_handle == tx_val_handle)
            {
                MC_BLE_TxCallback(ctxt->om);
            }
            break;
        }
        default:
            break;
    }
    return 0;
}


//蓝牙通讯处理函数
/*static int gatt_event_handler(uint16_t conn_handle, uint16_t attr_handle, 
                              struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) //写入特征值
    {
       if(attr_handle == rx_val_handle) //判断是否是RX特征
       {
           if(ctxt->om->om_data[0] ==0x00)
            {
                ESP_LOGI(TAG, "接收到数据: 0x00");
            }
            //这里可以根据需要处理接收到的数据
       }
       
    }
    else if(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) //读取特征值
    {
        if(attr_handle == tx_val_handle) //判断是否是TX特征
        {
            const char *value = "Hello from ESP32"; //要发送的数据
            os_mbuf_append(ctxt->om, value, strlen(value)); //将数据写入到输出缓冲区
        }
        
    }
    return 0;
}*/

//蓝牙连接处理函数                             
static int gap_event_hand(struct ble_gap_event *event, void *arg)
{
    //判断事件类型
    if (event->type == BLE_GAP_EVENT_CONNECT) 
    {
        //判断连接状态
        if (event->connect.status == 0) 
        {
            ESP_LOGI(TAG, "蓝牙连接成功");
            ble_adv_active = false; //连接成功，停止广播
        } 
        else 
        {
            ESP_LOGE(TAG, "连接失败，状态码: %d", event->connect.status);
            if (!ble_adv_active)//如果没有广播，重新启动广播
            {
                start_advertising();
            }
        }
    } 
    else if (event->type == BLE_GAP_EVENT_DISCONNECT) 
    {
        ESP_LOGI(TAG, "断开连接，原因: %d", event->disconnect.reason);
        if (!ble_adv_active)//如果没有广播，重新启动广播
        {
             start_advertising();
        }
        } 
    else if (event->type == BLE_GAP_EVENT_ADV_COMPLETE)
    {
        ESP_LOGI(TAG, "广播完成");
        ble_adv_active = false;
    }
    return 0;
} 




//服务UUID
static const  struct ble_gatt_svc_def gatt_svcs[] ={
    {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(SERVICE_UUID), //自定义服务UUID
    .characteristics = (struct ble_gatt_chr_def[])  //特征UUID
        {
            //RX特征,用于ESP接收数据
            {
                .uuid = BLE_UUID16_DECLARE(RX_UUID),
                .access_cb = gatt_event_handler,
                .flags = BLE_GATT_CHR_F_READ|BLE_GATT_CHR_F_WRITE,
                .val_handle = &rx_val_handle,
                .arg = NULL,
            },
            //TX特征,用于ESP发送数据
            {
                .uuid = BLE_UUID16_DECLARE(TX_UUID),
                .access_cb = gatt_event_handler,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &tx_val_handle,
                .arg = NULL,
            },
            {0}
        }
    },
    {0}
};

//
void start_advertising(void)
{
    //定义结构体
    struct ble_hs_adv_fields fields = {0};
    fields.name = (uint8_t *)DEVICE_NAME;//设置广播名称
    fields.name_len = strlen(DEVICE_NAME);//设置广播名称长度
    fields.name_is_complete = 1;//设置广播名称完整(0表示不完整，1表示完整)
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;//可以被扫描到|不支持经典蓝牙
    fields.tx_pwr_lvl_is_present = 1;//1广播信号携带发射功率信息，0不携带
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;//广播信号携带发射功率信息，自动计算
    
    //设置广播内容
    int rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "广播内容设置失败, rc=%d", rc);
        return;
    }

    //设置广播参数
    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,//可连接
        .disc_mode = BLE_GAP_DISC_MODE_GEN,//可被扫描到
        .itvl_min  = BLE_GAP_ADV_ITVL_MS(200),//广播间隔最小值
        .itvl_max  = BLE_GAP_ADV_ITVL_MS(500),//广播间隔最大值

    };
    rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, 
                          BLE_HS_FOREVER,
                          &adv_params, 
                          gap_event_hand, 
                          NULL);
    if (rc != 0) 
    {
        ESP_LOGE(TAG, "广播启动失败, rc=%d", rc);
    }
    else
    {
        ESP_LOGI(TAG, "广播启动成功");
        ble_adv_active = true;
    }
}

//自定义的MC_BLE_Init回调函数，这里是开启广播
static void on_sync(void)
{
    start_advertising();
    ESP_LOGI(TAG, "BLE start");
}


void host_task(void *arg)
{
   nimble_port_run(); //运行NimBle协议栈
}


void MC_BLE_Init(void)
{
    nimble_port_init(); //NimBle协议栈初始化
    ble_svc_gap_init(); //GAP服务初始化
    ble_svc_gatt_init();//GATT服务初始化
    ble_svc_gap_device_name_set(DEVICE_NAME); //设置设备名称
    ble_gatts_count_cfg(gatt_svcs);//配置服务
    ble_gatts_add_svcs(gatt_svcs); //添加服务

    ble_hs_cfg.sync_cb = on_sync; //设置同步回调函数

    nimble_port_freertos_init(host_task); //初始化NimBle协议栈的FreeRTOS任务

    ESP_LOGI(TAG, "BLE initialized");
}
