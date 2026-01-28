#include "Wifi_MQTT.h"
#include "mqtt_client.h"
#include "onenet_token.h"

#include <stdio.h>
#include "esp_log.h"
#include <stdint.h>
#include <string.h>

#define TAG "Onenet"

static esp_mqtt_client_handle_t mqtt_handle = NULL;

// 事件回调
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

// 初始化
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












