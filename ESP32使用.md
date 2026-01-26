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













































