#include "Wifi_MQTT.h"
#include "mqtt_client.h"
#include "onenet_token.h"
#include "onenet_dm.h"

#include <stdio.h>
#include "esp_log.h"
#include <stdint.h>
#include <string.h>

// MQTT的TAG
#define TAG "Onenet_mqtt"
//连接成功标志位
static bool onenet_connected_flg = false;
//mqtt连接客户端
static esp_mqtt_client_handle_t mqtt_handle = NULL;

// ========================== 中国移动-Onenet-MQTT服务 函数 ==========================

esp_err_t Send_To_Onenet(void) ;

// 事件回调
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) 
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Onenet mqtt connected");
            onenet_connected_flg = true;
            // 连接完成后先发送一次订阅和数据

            // 发送Topic
            onenet_subscribe(mqtt_handle ,onenet_connected_flg , ONENET_PRODUCT_ID, ONENET_DEVICE_NAME ); // 订阅上位机两种消息
            
            // 发送数据
            Send_To_Onenet() ;

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Onenet mqtt disconnected");
            ESP_ERROR_CHECK(esp_mqtt_client_reconnect(mqtt_handle));
            onenet_connected_flg = false;
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Onenet mqtt subscribed ack, msg_id=%d",event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "Onenet mqtt publish ack, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            // 上位机下行数据读取
            if(strstr(event->topic,"/property/set"))
            {
                cJSON *property_js = cJSON_Parse(event->data);
                cJSON *id_js = cJSON_GetObjectItem(property_js,"id");
                // 处理数据
                onenet_property_handle(property_js);
                // 下位机回显上位机发来的数据处理信息
                onenet_property_ack(mqtt_handle , cJSON_GetStringValue(id_js),200,"success" , ONENET_PRODUCT_ID , ONENET_DEVICE_NAME);
                cJSON_Delete(property_js);
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

// 初始化MQTT
esp_err_t Wifi_MQTT_Connect_Start(void)
{
    esp_mqtt_client_config_t mqtt_config ;
    memset(&mqtt_config , 0 , sizeof(esp_mqtt_client_config_t)) ;

    mqtt_config.broker.address.uri = "mqtt://mqtts.heclouds.com" ;
    mqtt_config.broker.address.port = 1883 ;

    mqtt_config.credentials.client_id = ONENET_DEVICE_NAME ;
    mqtt_config.credentials.username = ONENET_PRODUCT_ID ;

    static char token[256];

    dev_token_generate(token , SIG_METHOD_SHA256  , 2074859482 , ONENET_PRODUCT_ID , 
    ONENET_DEVICE_NAME , ONENET_PRODUCT_ACCESS_KEY);
    mqtt_config.credentials.authentication.password = token ;

    mqtt_handle = esp_mqtt_client_init(&mqtt_config) ;
    esp_mqtt_client_register_event(mqtt_handle , ESP_EVENT_ANY_ID , mqtt_event_handler , NULL) ;
    return esp_mqtt_client_start(mqtt_handle) ;
}

// 发送数据到云端
esp_err_t Send_To_Onenet(void)
{
    cJSON* property_js = onenet_generate_property_data();
    char* data = cJSON_PrintUnformatted(property_js);
    onenet_post_property_data(mqtt_handle, data, onenet_connected_flg, ONENET_PRODUCT_ID, ONENET_DEVICE_NAME);
    cJSON_free(data);
    cJSON_Delete(property_js);
    return ESP_OK;
}
