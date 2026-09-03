# XIAO ESP32-S3 Sense 引脚与板载资源

本文根据本工程中的 `引脚分配.png`、Arduino-ESP32 的 `XIAO_ESP32S3` 板级定义，以及 Seeed Studio 官方资料整理。

## 1. 使用结论

- 主板排针提供 **D0～D10，共 11 个 GPIO**。
- Sense 扩展板另外引出 **D11/GPIO42** 和 **D12/GPIO41**，但它们默认连接数字麦克风。
- D0～D5、D8～D10 支持 ADC 和电容触摸；D6、D7 不支持 ADC/触摸。
- D11、D12 虽然在部分 Seeed 引脚图中标为 `A11/A12`、`TOUCH12/TOUCH13`，但 GPIO41、GPIO42 实际不具备 ADC 或电容触摸能力，应按普通数字 GPIO 使用。
- 摄像头占用内部 GPIO10～18、GPIO38～40、GPIO47～48，但不占 D0～D10 排针。
- 使用板载 microSD 时，会占用 D2、D8、D9、D10。
- 使用板载数字麦克风时，会占用 D11、D12。
- ESP32-S3 没有片内 DAC；`analogWrite()` 在 Arduino-ESP32 中实际使用的是 PWM，不是真正的模拟电压输出。
- 所有 GPIO 的逻辑电平都是 **3.3 V**，不能直接输入 5 V。

## 2. 外部可用引脚总表

| 板上名称 | GPIO | 固定模拟/触摸能力 | Arduino 默认功能 | 其他可配置数字功能 | 板载占用及注意事项 |
|---|---:|---|---|---|---|
| D0 / A0 / T1 | 1 | ADC1_CH0、Touch 1 | 数字 IO | PWM、外部中断、UART、I²C、SPI、I²S、RMT 等 | 通常空闲 |
| D1 / A1 / T2 | 2 | ADC1_CH1、Touch 2 | 数字 IO | PWM、外部中断、UART、I²C、SPI、I²S、RMT 等 | 通常空闲 |
| D2 / A2 / T3 | 3 | ADC1_CH2、Touch 3 | 数字 IO | PWM、外部中断、UART、I²C、SPI、I²S、RMT 等 | 当前官方资料中为板载 SD 的 CS；GPIO3 也是启动配置引脚，复位时避免外部强拉 |
| D3 / A3 / T4 | 4 | ADC1_CH3、Touch 4 | 数字 IO | PWM、外部中断、UART、I²C、SPI、I²S、RMT 等 | 通常空闲 |
| D4 / A4 / T5 | 5 | ADC1_CH4、Touch 5 | I²C SDA | 数字 IO、PWM、外部中断、UART、SPI、I²S、RMT 等 | 默认 `SDA`，不用 I²C 时可作普通 GPIO |
| D5 / A5 / T6 | 6 | ADC1_CH5、Touch 6 | I²C SCL | 数字 IO、PWM、外部中断、UART、SPI、I²S、RMT 等 | 默认 `SCL`，不用 I²C 时可作普通 GPIO |
| D6 / TX | 43 | 无 ADC、无触摸 | UART TX | 数字 IO、PWM、外部中断、其他可路由数字外设 | 默认串口发送；使用 `Serial` 时避免复用 |
| D7 / RX | 44 | 无 ADC、无触摸 | UART RX；通用 SPI `SS` 别名 | 数字 IO、PWM、外部中断、其他可路由数字外设 | `SS=44` 只是 Arduino 通用 SPI 默认片选，不是板载 SD 的 CS |
| D8 / A8 / T7 | 7 | ADC1_CH6、Touch 7 | SPI SCK | 数字 IO、PWM、外部中断、UART、I²C、I²S、RMT 等 | 板载 SD 的 SCK |
| D9 / A9 / T8 | 8 | ADC1_CH7、Touch 8 | SPI MISO | 数字 IO、PWM、外部中断、UART、I²C、I²S、RMT 等 | 板载 SD 的 MISO |
| D10 / A10 / T9 | 9 | ADC1_CH8、Touch 9 | SPI MOSI | 数字 IO、PWM、外部中断、UART、I²C、I²S、RMT 等 | 板载 SD 的 MOSI |
| D11 | 42 | 无 ADC、无触摸 | 麦克风 PDM CLK | 数字 IO、PWM、外部中断、JTAG MTMS、其他数字外设 | 默认接麦克风；改作 GPIO 需要断开扩展板对应焊桥 |
| D12 | 41 | 无 ADC、无触摸 | 麦克风 PDM DATA | 数字 IO、PWM、外部中断、JTAG MTDI、其他数字外设 | 默认接麦克风；改作 GPIO 需要断开扩展板对应焊桥 |

这里的“UART、I²C、SPI、I²S、RMT 等”表示 ESP32-S3 可通过 GPIO Matrix 将多数数字外设信号路由到这些 GPIO。实际使用时仍要避免与表中的板载设备冲突，并在库的初始化函数中明确指定引脚。

## 3. Arduino 默认总线

### I²C

| 信号 | Arduino 名称 | GPIO | 排针 |
|---|---|---:|---|
| SDA | `SDA` | 5 | D4 |
| SCL | `SCL` | 6 | D5 |

默认初始化：

```cpp
Wire.begin();              // 使用 SDA=5、SCL=6
// Wire.begin(5, 6);       // 显式指定，效果相同
```

### SPI

| 信号 | Arduino 名称 | GPIO | 排针 |
|---|---|---:|---|
| SCK | `SCK` | 7 | D8 |
| MISO | `MISO` | 8 | D9 |
| MOSI | `MOSI` | 9 | D10 |
| 通用 SS | `SS` | 44 | D7 |

注意：SPI 的片选信号通常可以使用任意空闲输出 GPIO。`SS=GPIO44` 不代表板载 microSD 使用 GPIO44。

### UART

| 信号 | Arduino 名称 | GPIO | 排针 |
|---|---|---:|---|
| TX | `TX` | 43 | D6 |
| RX | `RX` | 44 | D7 |

ESP32-S3 的 UART 信号可以重新映射。例如：

```cpp
Serial1.begin(115200, SERIAL_8N1, 44, 43); // RX=44，TX=43
```

## 4. 板载摄像头占用

Sense 扩展板的摄像头使用 DVP 并行接口及 SCCB 控制总线：

| 摄像头信号 | GPIO | 说明 |
|---|---:|---|
| XCLK / XMCLK | 10 | 摄像头外部主时钟 |
| Y2 / D0 | 15 | 图像数据位 0 |
| Y3 / D1 | 17 | 图像数据位 1 |
| Y4 / D2 | 18 | 图像数据位 2 |
| Y5 / D3 | 16 | 图像数据位 3 |
| Y6 / D4 | 14 | 图像数据位 4 |
| Y7 / D5 | 12 | 图像数据位 5 |
| Y8 / D6 | 11 | 图像数据位 6 |
| Y9 / D7 | 48 | 图像数据位 7 |
| PCLK | 13 | 像素时钟 |
| VSYNC | 38 | 帧同步 |
| HREF | 47 | 行有效/行同步 |
| CAM_SCL / SIOC | 39 | SCCB 时钟 |
| CAM_SDA / SIOD | 40 | SCCB 数据 |
| PWDN | -1 | 未连接 |
| RESET | -1 | 未连接 |

这些引脚经扩展板内部连接到摄像头。启用摄像头时不要再把它们配置给其他外设。

## 5. 板载 microSD 占用

当前 Seeed 官方入门与引脚复用资料给出的板载 microSD 为 SPI 接口：

| microSD 信号 | GPIO | 对应排针 |
|---|---:|---|
| CS | 3 | D2 |
| SCK | 7 | D8 |
| MISO | 8 | D9 |
| MOSI | 9 | D10 |

因此：

- 不使用 microSD 时，D2、D8、D9、D10 均可恢复为普通 GPIO 或其他复用功能。
- 使用 microSD 时，可以让其他 SPI 设备共用 D8～D10，但每个设备必须使用独立 CS。
- 不要把 Arduino 头文件中的 `SS`（GPIO44）误当成板载 SD 的 CS。
- 部分较旧的 Seeed 示例曾使用 GPIO21 作为 `SD_PIN_CS`。如果你的扩展板硬件版本或配套示例与本文不一致，应优先按该版本原理图或官方示例确认 CS；当前官方总表标为 GPIO3。

## 6. 板载数字麦克风占用

| 麦克风信号 | GPIO | 扩展板引脚 |
|---|---:|---|
| PDM CLK | 42 | D11 |
| PDM DATA | 41 | D12 |

D11、D12 默认保留给 PDM 数字麦克风。如果需要将它们作为普通 GPIO，必须按 Seeed 官方说明断开 Sense 扩展板上的 J1/J2 麦克风连接焊桥；断开后麦克风不可用，重新焊接可恢复。

此外，当前 Arduino-ESP32 的 `XIAO_ESP32S3` 变体头文件没有定义 `D11` 和 `D12` 常量。代码中应直接使用 GPIO 编号，或自行定义：

```cpp
constexpr uint8_t PIN_D11 = 42;
constexpr uint8_t PIN_D12 = 41;
```

## 7. 其他板载资源

| 资源 | GPIO/连接 | 注意事项 |
|---|---|---|
| 用户 LED | ==GPIO21== | Arduino 名称为 `LED_BUILTIN`；==输出1熄灭，输出0点亮== |
| BOOT 按钮 | ==GPIO0== | 按下时通常为低电平；启动阶段用于进入下载模式，可以作为按键复用 |
| RESET 按钮 | CHIP_PU / EN | 硬件复位，不是普通 GPIO |
| 原生 USB | GPIO19/20 内部连接 | 用于 USB CDC/JTAG/OTG，不在两侧排针上 |
| Wi-Fi / BLE 天线 | U.FL/IPEX 天线座 | 不是 GPIO，使用无线功能前应正确连接天线 |
| 5V | VBUS | USB 5 V；IO 仍然只允许 3.3 V 电平 |
| 3V3 | 稳压输出 | 官方标称最多约 700 mA，但还需扣除主控、摄像头、SD 等板载负载 |
| GND | 地 | 外部模块必须共地 |

## 8. 按使用场景选择引脚

### 摄像头开启，不使用 SD 和麦克风

可优先使用 D0～D10；D11、D12 只有在断开麦克风焊桥后才适合作 GPIO。

### 摄像头和 SD 同时开启

- D2、D8、D9、D10 被 SD 占用。
- 可优先使用 D0、D1、D3、D4、D5、D6、D7。
- D4/D5 若还需要 I²C，则也应保留。
- D6/D7 若还需要硬件串口，则也应保留。

### 摄像头、SD、麦克风全部开启

| 状态 | 引脚 |
|---|---|
| 可优先作为普通 GPIO | D0、D1、D3、D4、D5、D6、D7 |
| SD 占用 | D2、D8、D9、D10 |
| 麦克风占用 | D11、D12 |
| 摄像头内部占用 | GPIO10～18、38～40、47、48 中上表所列引脚 |

“可优先作为普通 GPIO”不等于可以忽略默认总线：若程序同时启用 I²C 或 UART，还要分别保留 D4/D5 或 D6/D7。

## 9. 常见误区

1. `D8` 不是 GPIO8，而是 GPIO7；`D9` 才是 GPIO8，`D10` 是 GPIO9。
2. Arduino 名称、GPIO 编号、ADC 通道号和触摸通道号是四套不同编号。
3. `A8` 只是 Arduino 别名，实际连接 GPIO7；它不代表 ADC 通道 8。
4. D11/GPIO42、D12/GPIO41 不支持 ADC，图中的 `A11/A12` 标识不应直接用于 `analogRead()`。
5. ESP32-S3 没有真正的 DAC 输出；PWM 需要外接 RC 滤波后才能得到近似模拟电压。
6. 板载摄像头的 SCCB 总线 GPIO39/40 不是排针上的默认 I²C GPIO5/6。
7. 摄像头引脚名称 Y2～Y9 是数据位名称，不是 XIAO 的 D2～D9 排针。

## 10. 资料来源

- 本工程引脚图：[`引脚分配.png`](./引脚分配.png)
- [Seeed Studio：XIAO ESP32-S3 系列入门及完整引脚表](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- [Seeed Studio：XIAO ESP32-S3（Sense）引脚复用](https://wiki.seeedstudio.com/cn/xiao_esp32s3_pin_multiplexing/)
- [Seeed Studio：XIAO ESP32-S3 Sense 摄像头说明](https://wiki.seeedstudio.com/cn/xiao_esp32s3_camera_usage/)
- [Espressif Arduino-ESP32：XIAO_ESP32S3 `pins_arduino.h`](https://github.com/espressif/arduino-esp32/blob/master/variants/XIAO_ESP32S3/pins_arduino.h)

