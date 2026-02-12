#include "MPU6050.h"
#include "I2C.h"
// ====================== 宏定义MPU6050的寄存器信息,使得更好理解 ====================
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

// 加速度计量程对应寄存器值
#define ACCEL_2G        0x00    // ±2g
#define ACCEL_4G        0x08    // ±4g
#define ACCEL_8G        0x10    // ±8g
#define ACCEL_16G       0x18    // ±16g

// 陀螺仪量程对应寄存器值
#define GYRO_250        0x00    // ±250°/s
#define GYRO_500        0x08    // ±500°/s
#define GYRO_1000       0x10    // ±1000°/s
#define GYRO_2000       0x18    // ±2000°/s

// ==================== 重要:确定想要的量程 ====================
#define ACCEL_RANGE     ACCEL_2G
#define GYRO_RANGE      GYRO_250

// 宏定义确定满量程
/* ==================== 灵敏度宏（用于数据转换） ==================== */
#if   ACCEL_RANGE == ACCEL_2G
    #define ACCEL_SENSITIVITY   16384.0f   // LSB/g
#elif ACCEL_RANGE == ACCEL_4G
    #define ACCEL_SENSITIVITY   8192.0f
#elif ACCEL_RANGE == ACCEL_8G
    #define ACCEL_SENSITIVITY   4096.0f
#elif ACCEL_RANGE == ACCEL_16G
    #define ACCEL_SENSITIVITY   2048.0f
#endif

#if   GYRO_RANGE == GYRO_250
    #define GYRO_SENSITIVITY    131.0f     // LSB/(°/s)
#elif GYRO_RANGE == GYRO_500
    #define GYRO_SENSITIVITY    65.5f
#elif GYRO_RANGE == GYRO_1000
    #define GYRO_SENSITIVITY    32.8f
#elif GYRO_RANGE == GYRO_2000
    #define GYRO_SENSITIVITY    16.4f
#endif

// 参数配置:注意,引脚和OLED完全相同,共用一个IIC总线,要改一起改
#define MPU6050_ADDRESS 	0xD0		// MPU6050地址(AD0接地) !!!8位!!!
#define MPU6050_SPEED  		400000

// MPU6050设备句柄
i2c_master_dev_handle_t mpu_dev_handle;

// 外部引用参数
MPU6050_Raw_Data  	MPU_Raw_Data ;	// 最初的角度

// ============================== 初始化配置 ==============================
// 初始化MPU6050相关配置
void MPU6050_Init(void)
{
	// ======================= 引脚初始化 =======================
	//配置I2C总线
    I2C_Master_Init() ;
    //配置I2C从机设备
    i2c_device_config_t mpu_dev_cfg = 
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  		//设置设备地址长度为7位
        .device_address = MPU6050_ADDRESS >> 1,         //指定设备地址
        .scl_speed_hz = MPU6050_SPEED,                  //设置I2C时钟速度
        .flags.disable_ack_check = false,       		//启用ACK检查
    };
	//将设备添加到I2C总线并获取设备句柄
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &mpu_dev_cfg, &mpu_dev_handle));
	// ======================= 寄存器初始化 =======================
	// 此处仅配置了部分重要的寄存器
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		// 电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		// 电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		// 采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			// 配置寄存器，配置DLPF
	
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, GYRO_RANGE);	  	// 陀螺仪配置寄存器
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, ACCEL_RANGE);	// 加速度计配置寄存器
}
// ============================== 基础函数 ==============================

// 写入单个数据
void MPU6050_WriteReg(uint8_t RegAddress , uint8_t Data)
{
	// 硬件IIC
	uint8_t writebuffer[2];
    writebuffer[0] = RegAddress;
    writebuffer[1] = Data;
    ESP_ERROR_CHECK(i2c_master_transmit(mpu_dev_handle, writebuffer, 2, 1000));
}

// 读取单个数据
// uint8_t MPU6050_ReadReg(uint8_t RegAddress)
// {
// 	// 读取的数据
// 	uint8_t Data ;
	
// 	// 硬件IIC
// 	ESP_ERROR_CHECK(i2c_master_transmit_receive(mpu_dev_handle, &RegAddress, 1, &Data, 1, 1000));
// 	// 返回数据

// 	return Data;
// }
// ============================== 上层配置 ==============================

// // 得到MPU6050的地址
// uint8_t MPU6050_GetID(void)
// {
// 	return MPU6050_ReadReg(MPU6050_WHO_AM_I) ;
// }
// 原始数据更新
void MPU6050_Update_Data(void)
{
	// 数据接收区,连续读取
	uint8_t buf[14];
	// 硬件IIC连续读取
	uint8_t  write_buffer = MPU6050_ACCEL_XOUT_H ;
	ESP_ERROR_CHECK(i2c_master_transmit_receive(mpu_dev_handle, &write_buffer, 1, buf, 14, 1000));
	
	// 数据处理
	// 得到加速度
	MPU_Raw_Data.AX  = ((int16_t)(buf[0] << 8 | buf[1])) * 1.0 / ACCEL_SENSITIVITY ;
	MPU_Raw_Data.AY  = ((int16_t)(buf[2] << 8 | buf[3])) * 1.0 / ACCEL_SENSITIVITY ;
	MPU_Raw_Data.AZ  = ((int16_t)(buf[4] << 8 | buf[5])) * 1.0 / ACCEL_SENSITIVITY ;
	// 得到角速度(跳过了温度)
	MPU_Raw_Data.GX = ((int16_t)(buf[8]  << 8 | buf[9]))  * 1.0 / GYRO_SENSITIVITY ;
	MPU_Raw_Data.GY = ((int16_t)(buf[10] << 8 | buf[11])) * 1.0 / GYRO_SENSITIVITY ;
	MPU_Raw_Data.GZ = ((int16_t)(buf[12] << 8 | buf[13])) * 1.0 / GYRO_SENSITIVITY ;
}
