#ifndef __WIFI_MQTT_H
#define __WIFI_MQTT_H

#include "esp_err.h"

// 产品ID
#define ONENET_PRODUCT_ID "95Mlb1RNJC"

// 产品密钥
#define ONENET_PRODUCT_ACCESS_KEY "D2uRGwU3D2UFlYmNUbWTJy3VS3e90Wutk4e1SZ8DdWE="

// 设备名称
#define ONENET_DEVICE_NAME "ESP32-01"

esp_err_t Wifi_MQTT_Connect_Start(void);

#endif




