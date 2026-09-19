# OTA 智能控制终端

这是一个基于 `STM32F103C8T6` 的嵌入式学习与实践项目，按照功能递进的方式实现：

- FreeModbus 从裸机移植到 STM32；
- 在 Modbus 工程上加入 FreeRTOS；
- 通过 ESP8266 Wi-Fi 模块和 4G 模块接入 MQTT；
- 将 Modbus、MQTT 和 CANopen 放入 FreeRTOS 任务中运行；
- 使用 BootLoader、双应用分区和 MQTT 分包传输实现 OTA 远程升级。

项目中的各目录是相互独立的 STM32CubeMX/Keil 工程，适合按照 `01` 到 `08` 的顺序学习。由于 STM32F103C8T6 的 Flash 和 RAM 资源有限，当前工程重点放在 Modbus、MQTT、CANopen、FreeRTOS 和 OTA，其他协议未纳入本仓库。

## 功能概览

所有主要工程都包含以下基础功能：

- STM32 HAL 初始化和时钟配置；
- OLED 显示；
- AHT20 温湿度采集；
- INA226 电压、电流和功率采集；
- Modbus 数据轮询与设备控制；
- GPIO 控制 LED、蜂鸣器和继电器；
- 通过串口连接外部通信模块。

## 工程目录

| 工程 | 主要内容 | 关键变化 |
| --- | --- | --- |
| [`01-modbus`](./01-modbus) | 裸机 Modbus | 移植 FreeModbus，主循环中调用 `eMBPoll()` 和 `Modbus_Parse()` |
| [`02-modbus-freertos`](./02-modbus-freertos) | Modbus + FreeRTOS | 创建 `ModbusTask`，在任务中运行 Modbus 轮询 |
| [`03-mqtt-wifi`](./03-mqtt-wifi) | ESP8266 Wi-Fi + MQTT | 使用 UART3 DMA 接收模块数据，支持 MQTT 上下行 JSON 数据 |
| [`04-mqtt-4g`](./04-mqtt-4g) | 4G + MQTT | 增加 ML307 4G 模块连接流程，同时保留 ESP8266 代码路径 |
| [`05-mqtt-freemodbus`](./05-mqtt-freemodbus) | Modbus + MQTT + FreeRTOS | 创建 `ModbusTask` 和 `MQTTTask`，周期上传设备数据 |
| [`06-canopen`](./06-canopen) | CANopen | 集成 CanFestival，使用 CAN1 和定时器驱动 CANopen |
| [`07-canopen-freertos`](./07-canopen-freertos) | CANopen + Modbus + MQTT + FreeRTOS | 创建 Modbus、MQTT、CANopen 三个任务 |
| [`08-boot-ota`](./08-boot-ota) | BootLoader + OTA | 包含 Boot、`app1-1` 和 `app1-2` 三个工程，实现双分区固件升级 |

## 硬件与软件环境

### 硬件

- MCU：STM32F103C8T6；
- 外部传感器：AHT20、INA226；
- 显示：I2C OLED；
- 通信接口：RS485、UART、CAN；
- 网络模块：ESP8266 Wi-Fi 或 ML307 4G；
- Flash 页大小：`1 KB`。

工程中的常用引脚和外设由 `.ioc` 文件配置，具体以对应目录下的 CubeMX 配置为准。

### 软件

- STM32CubeMX；
- STM32Cube FW_F1 `V1.8.6`；
- Keil MDK-ARM `V5.32`；
- 工程使用 STM32 HAL、CMSIS、FreeModbus、FreeRTOS 和 CanFestival。

## 编译与烧录

1. 使用 Keil 打开对应目录下的 `MDK-ARM/*.uvprojx` 工程。
2. 确认目标芯片为 `STM32F103C8`，并检查串口、传感器、通信模块和电源连接。
3. 编译工程并生成 HEX/BIN 文件。
4. 按照工程顺序烧录和验证功能。

`01` 到 `07` 工程的程序从 Flash 起始地址运行。`08-boot-ota` 的烧录顺序和地址不同，必须按照下面的分区方式处理。

## MQTT 配置

`03` 到 `07` 工程中的 MQTT 参数位于各自的 `Core/Inc/mqtt.h` 和 `Core/Src/mqtt.c`。代码当前默认使用：

- MQTT Broker：`broker.emqx.io`
- MQTT 端口：`1883`
- 设备标识：读取 STM32 UID 生成
- 普通上行主题：`STM32V9/UPLoad/<设备ID>`
- 普通下行主题：`STM32V9/DownLoad/<设备ID>`

`04-mqtt-4g` 和后续工程通过 `MQTT_WIFI_4G_ENABLE` 选择网络路径：

```c
// 1: ESP8266 Wi-Fi
// 0: ML307 4G
#define MQTT_WIFI_4G_ENABLE 0
```

实际使用时需要根据网络模块型号、SIM 卡/APN、服务器地址、端口和认证信息修改代码。仓库中的服务器地址和主题属于示例配置，不应直接用于生产环境。

## OTA 工程

`08-boot-ota` 包含三个独立工程：

- `boot`：上电后读取升级标志，决定正常启动或执行固件搬运，然后跳转到 APP1；
- `app1-1`：当前运行的应用程序，链接地址为 `0x08002000`；
- `app1-2`：下载固件暂存区，代码功能与 APP1 基本一致，用于接收下一版本固件。

### Flash 分区

| 区域 | 起始地址 | 大小 | 说明 |
| --- | ---: | ---: | --- |
| BootLoader | `0x08000000` | `0x2000`（8 KB） | 启动、校验启动标志、搬运固件、跳转 APP |
| Application 1 | `0x08002000` | `0x7000`（28 KB） | 当前运行的应用 |
| Application 2 | `0x08009000` | `0x7000`（28 KB） | OTA 下载暂存区 |
| APP2 标志位 | `0x0800FFFC` | 4 B | `0xAAAAAAAA` 表示下次启动升级 |

上述地址来自 `08-boot-ota/*/Core/Inc/boot.h`。APP 工程的链接脚本将应用镜像按 `0x08002000` 链接，APP2 镜像下载到暂存区后由 BootLoader 复制到 APP1，因此不要直接将 APP2 作为独立运行程序跳转执行。

### OTA 升级流程

1. APP1 通过 MQTT 接收服务器下发的固件信息，包括版本、大小和 CRC32。
2. APP1 按 `256` 字节分包请求固件数据。
3. 每收到一包数据就写入 Application 2，并增量计算 CRC32。
4. 固件接收完成后校验 CRC32。
5. 校验成功后，在 APP2 末尾写入 `0xAAAAAAAA`，然后软件复位。
6. BootLoader 读取升级标志，将 APP2 内容复制到 APP1。
7. BootLoader 清理源区域并跳转到 APP1。

OTA 相关 MQTT 主题定义在 `08-boot-ota/app1-1/Core/Inc/mqtt.h`：

- `v1/devices/me/rpc/request/+`：接收设备控制命令；
- `v1/devices/me/attributes`：接收 OTA 固件信息；
- `v2/fw/response/+/chunk/#`：接收固件分包；
- `v1/devices/me/telemetry`：上传设备数据；
- `v2/fw/request/+/chunk/#`：请求固件分包。

## 代码结构

每个常规工程主要由以下部分组成：

```text
工程目录/
├── Core/
│   ├── Inc/              头文件和外设接口
│   └── Src/              应用逻辑、驱动和中断处理
├── Drivers/              CMSIS 和 STM32 HAL
├── FreeModbus/           FreeModbus 协议栈
├── CanFestival/          CANopen 协议栈（06、07）
├── Middlewares/          FreeRTOS（02、05、07）
├── MDK-ARM/              Keil 工程和链接脚本
└── *.ioc                 STM32CubeMX 配置文件
```

`08-boot-ota` 的 `boot`、`app1-1` 和 `app1-2` 目录分别是独立工程，不能把它们当作一个普通单工程直接编译。

## 注意事项

- 修改 `.ioc` 后重新生成代码时，应保留 `USER CODE` 区域中的应用逻辑。
- Modbus、MQTT 和 CANopen 共用串口、定时器、DMA 或中断资源时，需要根据实际硬件调整优先级和初始化顺序。
- MQTT 接收逻辑依赖串口 DMA、空闲中断和队列，通信模块的波特率、接线和透传模式必须匹配。
- OTA 固件大小不能超过 `Application_Size`，并且必须与服务端提供的 CRC32 算法和字节序一致。
- 修改 BootLoader 或应用分区地址时，必须同步修改 `boot.h`、Keil 链接脚本和烧录配置。
- 工程目录中可能包含 Keil 编译生成的中间文件，重新构建前应确认当前使用的是目标工程和正确的链接脚本。
