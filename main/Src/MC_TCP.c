#include "MC_TCP.h"
#include <string.h>

#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_ip_addr.h"

#include "wifi_sta.h"
#include "MC_Protocol.h"



static const char *TAG = "MC_TCP";

//TCP监听端口
//Mod连接这个端口
#define TCP_PORT 12345


//IP端口
static uint16_t tcp_port = 12345;
//IP地址
esp_ip4_addr_t device_ip;

//保存MOD的IP地址
struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);




//UDP服务器任务
static void MC_UDP_Discover_Task(void *arg)
{
    int sock;
    struct sockaddr_in server;
    sock = socket(
        AF_INET,
        SOCK_DGRAM,
        0
    );

    server.sin_family =
        AF_INET;
    server.sin_port =
        htons(8888);
    server.sin_addr.s_addr =
        INADDR_ANY;
    bind(
        sock,
        (struct sockaddr *)&server,
        sizeof(server)
    );

    while(1)
    {
        char buffer[64] = {0};
        ESP_LOGI(TAG,"Waiting UDP...");
        recvfrom(
            sock,
            buffer,
            sizeof(buffer)-1,
            0,
            (struct sockaddr *)&client_addr,
            &addr_len
        );

        if(strcmp(buffer,
                  "MC_DISCOVER")==0)
        {
            //回复IP和TCP端口
            char reply[64];
            sprintf(
                reply,
                "MC_DEVICE|"IPSTR"|%d",
                IP2STR(&device_ip),
                tcp_port
            );
            sendto(
                sock,
                reply,
                strlen(reply),
                0,
                (struct sockaddr *)&client_addr,
                addr_len
            );
            ESP_LOGI(TAG,"Send discover reply:%s",reply);
        }
    }
}

//TCP服务器任务
static void MC_TCP_Server_Task(void *arg)
{

    int server_fd;
    int client_fd;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t addr_len =
        sizeof(client_addr);
    /*
        创建TCP Socket
        AF_INET:
            IPv4
        SOCK_STREAM:
            TCP协议
    */
    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(server_fd < 0)
    {
        ESP_LOGE(TAG,
                 "socket create failed");
        vTaskDelete(NULL);
    }

    /*
        配置服务器地址
        INADDR_ANY:
            监听本机所有IP
        例如：
            ESP获取到
            192.168.1.50
            那么：
            192.168.1.50:12345
            可以连接
    */
    server_addr.sin_family =
        AF_INET;
    server_addr.sin_port =
        htons(TCP_PORT);
    server_addr.sin_addr.s_addr =
        INADDR_ANY;

    /*
        绑定端口
        相当于告诉系统：
        我要占用12345端口
    */
    if(bind(server_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG,
                 "bind failed");
        close(server_fd);
        vTaskDelete(NULL);
    }

    /*
        开始监听
        这里之后：
        ESP进入等待状态
        等待电脑连接
    */
    listen(server_fd,1);
    ESP_LOGI(TAG,
             "TCP Server Start Port:%d",
             TCP_PORT);
    while(1)
    {
        /*
            accept()
            阻塞等待客户端连接
            没有人连接：
                卡在这里
            Mod连接：
                返回client_fd
        */
        client_fd =
            accept(server_fd,
            (struct sockaddr *)&client_addr,
            &addr_len);
        if(client_fd < 0)
        {
            continue;
        }
        ESP_LOGI(TAG,
                 "Client Connected");
        /*
            连接成功
            开始收数据
        */
        while(1)
        {
            uint8_t buffer[128];
            int len =
                recv(
                    client_fd,
                    buffer,
                    sizeof(buffer)-1,
                    0
                );
            /*
                len > 0
                收到数据
                len = 0
                对方关闭连接
                len < 0
                错误
            */
            if(len <= 0)
            {
                ESP_LOGW(TAG,
                         "Client disconnected");
                break;
            }
            //送去协议解析
            MC_Stream_Parse(buffer,len);
            //字符串结尾--改u8后用不着了
            //buffer[len]=0;
            //ESP_LOGI(TAG,"RX:%s",buffer);
        }
        //关闭客户端连接
        close(client_fd);
    }

}







//UDP+TCP启动函数
void MC_TCP_Server_Start(void)
{
    //UDP发现任务
    xTaskCreate(
        MC_UDP_Discover_Task,
        "MC_UDP",
        4096,
        NULL,
        5,
        NULL
    );

    //TCP服务器任务
    xTaskCreate(
        MC_TCP_Server_Task,
        "MC_TCP",
        4096,
        NULL,
        5,
        NULL
    );

}