#include "I2C.h"

// IIC总线配置
#define I2C_PORT    I2C_NUM_0
#define I2C_SCL    	GPIO_NUM_1
#define I2C_SDA    	GPIO_NUM_2

//I2C总线句柄
i2c_master_bus_handle_t i2c_bus_handle;

void I2C_Master_Init(void)
{
    // 配置初始化flag,不允许重复初始化
    static bool I2C_Init_Flag = false ;
    if (I2C_Init_Flag)
    {
        return ;
    }
    //配置I2C总线
    i2c_master_bus_config_t i2c_mst_cfg = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,      		//使用默认时钟源
        .i2c_port = I2C_PORT,                           //指定I2C端口号
        .scl_io_num = I2C_SCL,                      	//指定SCL引脚号
        .sda_io_num = I2C_SDA,                      	//指定SDA引脚号
        .glitch_ignore_cnt = 7,                 		//设置毛刺忽略计数
        .flags.enable_internal_pullup = true,  			//启动内部上拉电阻
    };
	
    //创建I2C总线并获取句柄
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_cfg, &i2c_bus_handle));
    I2C_Init_Flag = true ;  // 初始化完成
}
