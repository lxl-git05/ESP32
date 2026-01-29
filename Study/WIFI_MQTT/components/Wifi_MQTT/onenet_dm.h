#ifndef __ONENET_DM_H
#define __ONENET_DM_H

#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "mqtt_client.h" 
#include <stdbool.h>

// =========================== 物模型数据处理函数 ===========================

// 处理onenet下行的数据
void onenet_property_handle(cJSON* property_js);

// 生成上报给onene的所有数据的cJSON对象
cJSON* onenet_generate_property_data(void);

// 上行数据发送:data是发送的内容,在这里和topic一起发送
esp_err_t onenet_post_property_data(esp_mqtt_client_handle_t onenet_client , const char* data , bool Onenet_connected_flg , const char* product_id , const char* device_name);

// =========================== 物模型订阅处理函数 ===========================

// 订阅来自上位机的topic: 1. 上位机收到信息后发来的回显 2. 上位机发来的数据处理信息
esp_err_t onenet_subscribe(esp_mqtt_client_handle_t onenet_client , bool Onenet_connected_flg , const char* product_id , const char* device_name);

// 下位机回显上位机发来的数据处理信息
void onenet_property_ack(esp_mqtt_client_handle_t onenet_client , const char* id,int code,const char* message , const char* product_id , const char* device_name);

#endif
