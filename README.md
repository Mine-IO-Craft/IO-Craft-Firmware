# Orange MC 固件（ESP32-S3）

面向 Minecraft IO-Craft Mod 的 ESP32-S3 外设桥接固件。固件通过 BLE / UART / TCP 接收来自上位机的数据包，将游戏内数据（生命值、饥饿值、坐标、等级、难度、时间、天气、速度等）和控制指令（WS2812 灯带、GPIO、PWM）映射到真实硬件，并提供 WiFi 配网能力。

> 与 IO-Craft Mod 的具体对接说明待补充（由项目作者确认后更新本节）。

## 功能

- **BLE GATT 从机**（NimBLE）
  - 服务 UUID：`0xFFF0`
  - RX 特征 `0xFFF1`（写入，接收协议数据包）
  - TX 特征 `0xFFF2`（读取 / 通知，发送数据）
  - 广播设备名：`Orange_Ble`
- **UART**：UART1，115200 8N1，TX=GPIO17，RX=GPIO18（RX 内部上拉），接收协议数据包
- **WiFi STA**：启动时从 NVS 读取已保存的 SSID / 密码并自动连接
- **AP 配网**：无已保存配置时进入 AP 模式
  - SSID：`Orange-AP`，密码：`12345678`
  - 内置 Web 配网页面（默认网关 `192.168.4.1`），`POST /wifi` 提交 `ssid` / `password`，连接成功（Got IP）后保存到 NVS
- **UDP 发现**：监听 UDP 8888，收到 `MC_DISCOVER` 后回复 `MC_DEVICE|<设备IP>|<TCP端口>`
- **TCP 服务器**：监听 12345，按协议解析收到的数据包
- **协议控制**：WS2812（0x01）、GPIO 输出 / 开漏 / 上下拉（0x10 0x00）、PWM（0x10 0x02）

## 支持的硬件

- 芯片：ESP32-S3（当前 `sdkconfig` 按 2MB Flash、DIO、80MHz 配置）
- WS2812 LED：GPIO48（定义于 `main/Inc/MC_Task.h` 的 `BLINK_GPIO`）
- UART1：TX=GPIO17，RX=GPIO18

## 环境依赖

- ESP-IDF：≥ 5.0（开发验证版本：6.0.1）
- 组件：`espressif/led_strip` 3.0.3（Apache-2.0，由 IDF 组件管理器自动拉取，声明于 `main/idf_component.yml`）

## 编译与烧录

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p <PORT> flash monitor
```

> 说明：构建参数由 `sdkconfig.defaults` 提供（esp32s3 / 2MB / DIO / 80MHz / 单分区大容量 / BLE-NimBLE）。首次构建如本地无 sdkconfig，建议先执行 `idf.py set-target esp32s3` 生成。

## 通信协议（MC_Protocol）

数据包格式（3 字节包头 + 数据）：

```
[module:1B] [command:1B] [length:1B] [data:length B]
```

`length` 最大 244 字节。多包可连续拼接在同一个流中解析。

已注册的模块 / 命令：

| 模块 | 命令 | 说明 |
| --- | --- | --- |
| 0x00 系统 | 0x00 VERSION / 0x01 RESET / 0x02 DEFAULT / 0x03 STRBLE / 0x04 STRWIFI | 版本查询 / 复位 / 恢复出厂（后三项为预留接口） |
| 0x01 WS2812 | 0x00 SET_NUM（data[0]=灯珠数量） | 初始化灯带 |
| 0x01 WS2812 | 0x01 SET_RGB（index, R, G, B 共 4 字节） | 设置单灯颜色 |
| 0x02 MC 数据 | 0x00 HEALTH / 0x01 FOOD / 0x02 SATURATION / 0x03 COORDINATE / 0x05 LEVEL / 0x06 DIFFICULTY / 0x07 TIME / 0x08 WEATHER / 0x09 SPEED / 0x0A DAY / 0x0B EXP | 游戏数据上报（当前实现为打印日志） |
| 0x10 GPIO | 0x00 OUT_SET（gpio, level, open_drain, pull_up 共 4 字节） | GPIO 输出设置 |
| 0x10 GPIO | 0x02 PWM（gpio, timer, channel, freq 高字节在前, duty 高字节在前 共 7 字节） | PWM 设置 |

当前未实现：0x02 0x04 BIOME、0x10 0x01 GPIO 输入。

## 目录结构

```
├── CMakeLists.txt
├── main/
│   ├── main.c                 # 入口：初始化 UART / BLE / WiFi / TCP / Handler
│   ├── Inc/                   # 头文件
│   └── Src/
│       ├── MC_Protocol.c      # 协议解析与分发
│       ├── MC_Handler.c       # 命令处理
│       ├── MC_Task.c          # 硬件动作（WS2812 / GPIO / PWM）
│       ├── MC_Uart.c          # UART 接收
│       ├── MC_Ble.c           # BLE GATT 服务
│       ├── MC_Wifi.c / wifi_sta.c / wifi_ap.c / web_server.c   # WiFi 与配网
│       └── MC_TCP.c           # TCP 服务器 + UDP 发现
```

## 已知限制

- WS2812 初始化在 `app_main` 中处于注释状态（源码注释记录"实测有 bug，搁置"），当前通过协议命令 `0x01 0x00` 运行时初始化。
- 蓝牙与 WiFi 共用天线，同时开启时存在相互干扰风险（源码注释已说明）。
- UART / TCP 属于流式通道，当前 `MC_Stream_Parse` 对跨包残留数据未做缓冲，分包到达时尾部数据会被丢弃。
- MCDATA 各命令当前仅打印日志，尚未接入具体硬件动作。

## 第三方组件与参考

- `espressif/led_strip`（Apache-2.0）：WS2812 驱动，通过 IDF 组件管理器引入。
- BLE 部分参考 ESP-IDF `bleprph` 示例（Apache-2.0）的 NimBLE 使用方式编写。

## License

Apache-2.0，见 [LICENSE](LICENSE)。
