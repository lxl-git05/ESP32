#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "driver/i2c_master.h"
#include "esp_err.h"
#include <esp_log.h>

//I2C总线句柄
extern i2c_master_bus_handle_t i2c_bus_handle;

// IIC总线初始化
void I2C_Master_Init(void);

#endif
