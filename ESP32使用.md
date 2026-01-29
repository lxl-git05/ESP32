[toc]

## 1. 创建工程

<img src="ESP32使用.assets/image-20260126202547652.png" alt="image-20260126202547652" style="zoom:67%;" />

* 

![image-20260126202704481](ESP32使用.assets/image-20260126202704481.png)

* 

![image-20260126202759148](ESP32使用.assets/image-20260126202759148.png)

* 系统配置

![image-20260126203355076](ESP32使用.assets/image-20260126203355076.png)

![image-20260126203446417](ESP32使用.assets/image-20260126203446417.png)

![image-20260126203525968](ESP32使用.assets/image-20260126203525968.png)

* 然后就可以构建项目并且烧录程序了

## 2. 引入文件

* main层:CMakeLists.txt保持原样
* 引入库层：比如`MyLED`库，那么`cmake`需要：

```c
# 通过idf_component_register函数注册组件，相当于实现了自己的自建库函数，记得SRCS后面改名哦.c
idf_component_register(
    SRCS "MyLED.c"			# 库函数.c
    INCLUDE_DIRS "."
    REQUIRES driver			# 因为MyLED库使用了GPIO库,所以需要声明驱动,而GPIO属于driver文件,所以声明driver即可继承
)
# 这里还要进行组件依赖(main函数不需要,因为做了特殊处理，但是其他自设库需要进行这个操作)
# 组件依赖还有传递性,比如MyLED依赖esp_driver_gpio,那么esp_driver_gpio也需要依赖其他组件，比如esp_system,那么esp_system也需要依赖其他组件，以此类推
# 所以在idf_component_register函数中，REQUIRES后面可以写多个组件名，用空格隔开，依赖关系会自动递归处理

# 还有很多组件被存在了云端
# 自己去官网找,然后指令执行
```



## 3. GPIO输出模式-点亮LED

```c
#include "driver/gpio.h"
```

* 初始化GPIO

```c
void led_init(void)
{
    gpio_config_t LED_cfg = {
		// 注意：Dx口和GPIOx不同步,需要查看引脚分配图
        .pin_bit_mask = 1ULL << GPIO_NUM_2,     // GPIO口,需要使用掩码 1ULL << GPIO_NUM_2

        .mode = GPIO_MODE_OUTPUT,               // GPIO模式

        .pull_up_en = GPIO_PULLUP_DISABLE,      // GPIO上下拉:失能

        .pull_down_en = GPIO_PULLDOWN_DISABLE,  // GPIO上下拉:失能

        .intr_type = GPIO_INTR_DISABLE,         // GPIO中断类型:失能

    };

    gpio_config(&LED_cfg);
}
```

* 设置输出状态

```c
// 这里就不需要使用位掩码了
gpio_set_level(GPIO_NUM_2, led_level);
```



## 4. PWM输出

```c
#include "driver/ledc.h"
```



## 5. 硬件IIC驱动OLED

* 核心IIC通信函数

```c
// OLED写命令
void OLED_WriteCommand(uint8_t Command)
{
    uint8_t writebuffer[2];

    writebuffer[0] = 0x00;
    writebuffer[1] = Command;
    ESP_ERROR_CHECK(i2c_master_transmit(oled_dev_handle, writebuffer, 2, -1));
}

// OLED写数据
void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
	uint8_t i;
    uint8_t writebuffer[Count+1];

    writebuffer[0] = 0x40;

    for (i = 0; i < Count; i ++)
	{
		writebuffer[i+1] = Data[i];
	}
    ESP_ERROR_CHECK(i2c_master_transmit(oled_dev_handle, writebuffer, Count+1, -1));
}
```

* IIC 初始化函数
  * 配置IIC总线：所有IIC可以通用
  * 配置从机设备,这里只有OLED，其实还可以有更多，都需要配置从机，记住从机7位地址取原始7位地址

```c
//配置I2C总线
i2c_master_bus_config_t oled_i2c_mst_cfg = 
{
    .clk_source = I2C_CLK_SRC_DEFAULT,      		//使用默认时钟源
    .i2c_port = OLED_I2C_PORT,                      //指定I2C端口号
    .scl_io_num = OLED_SCL,                      	//指定SCL引脚号
    .sda_io_num = OLED_SDA,                      	//指定SDA引脚号
    .glitch_ignore_cnt = 7,                 		//设置毛刺忽略计数
    .flags.enable_internal_pullup = true,  			//禁用内部上拉电阻（前提是已经外部上拉）
};

//创建I2C总线并获取句柄
ESP_ERROR_CHECK(i2c_new_master_bus(&oled_i2c_mst_cfg, &oled_bus_handle));

//配置I2C从机设备
i2c_device_config_t oled_dev_cfg = 
{
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,  	 //设置设备地址长度为7位
    .device_address = OLED_ADD >> 1,             //指定设备地址,这是OLED是8位,所以>>1
    .scl_speed_hz = OLED_SPEED,                  //设置I2C时钟速度
    .flags.disable_ack_check = false,       	 //启用ACK检查
};

//将设备添加到I2C总线并获取设备句柄
ESP_ERROR_CHECK(i2c_master_bus_add_device(oled_bus_handle, &oled_dev_cfg, &oled_dev_handle));
```



## 6. MQTT协议通信

### 6-1 Onenet_dm数据处理

* 头文件

```c
// 初始化

// Handle JcSON数据处理

// 上传数据到云端
```

* 源文件解析

```c
// 初始化函数


```



### 6-2 `Onenet`与`ESP32`通信规则

### 6-2-1 ==总体概述：4种对话方式==

| 名词     | 谁发起 | 是否要回复 | 场景     |
| -------- | ------ | ---------- | -------- |
| 属性上报 | 设备   | ❌          | 定时数据 |
| 属性设置 | 平台   | ✅          | 控制设备 |
| 属性获取 | 平台   | ✅          | 查询状态 |
| 事件上报 | 设备   | ❌          | 异常通知 |

* ==对话原则(自己的理解)==

* ESP32端和`Onenet`端(云端)进行对话 ， 以下简称ESP32为**下位机** ， 云端为**上位机**
* **ESP32**发送的东西为**上行**，**云端**发送的东西为**下行**



* <u>==下位机工程配置==</u>（简化版，1个主题发送，2个主题订阅）
  * 下位机发送订阅主题：属性上报主题（这样上位机就能知道下位机上报的数据内容了）
  * 下位机接收订阅主题：属性上报回显（看看自己发的东西上位机是不是接收正确），属性设置主题（接收上位机的命令，改变自己的参数）
  * 上位机自己会配置，我们不用管

***

* ==规则==
* 双方发布的数据无非就两类：topic和数据内容，topic为订阅内容(**订阅了才能接收到数据**，否则接受不到)
  * 下位机发送的消息
    * 上位机必须订阅！并且必须响应，发送响应信息
  * 上位机发送的消息
    * 下位机不必须订阅，并且不必须相应，不必须发送响应信息
    * 建议订阅上位机的主动请求如属性设置，属性获取
    * 至于上位机的相应信息，下位机有时间可以订阅来检查一下上位机是否接收成功，也可以不订阅，鸟都不鸟上位机发了什么

***

### 6-2-2 **设备属性上报**：==设备 → 云==(上行)

* 其他3种类似，自己去官网看吧:[文档中心](https://iot.10086.cn/doc/aiot/fuse/detail/902)

* 设备主动告诉云平台：我现在是什么状态

***

* 请求topic：
  * `{pid}`为设备ID
  * `{device-name}`为设备名称
  * **上面两个参数改成自己的**

```c
$sys/{pid}/{device-name}/thing/property/post
```

* `OneJSON`数据格式:

```c
{
  "id": "123",
  "version": "1.0",
  "params": {	// 参数键值对，最重要
    "Power": {
      "value": "on",
      "time": 1524448722123	// 时间戳，可以判断消息发送的时间
    },
    "WF": {
      "value": 23.6,
      "time": 1524448722123
    }
  }
}
```

***

* 响应Topic:

```c
$sys/{pid}/{device-name}/thing/property/post/reply
```

* 数据

```c
{
  "id": "123",
  "code":200,		// 结果状态码:200代表成功了
  "msg":"xxxx"
}
```

***

### 6-2-3 **设备属性设置：**==云 → 设备==（下行）

* 云平台下命令，让设备“改状态”



### 6-2-4 **设备属性获取：**==云 → 设备 → 云==

* 云平台主动问：你现在的状态是多少？设备必须回答



### 6-2-5 **事件上报**：==设备 → 云==（上行）

* 设备主动告诉云：发生了一件“重要事情”









