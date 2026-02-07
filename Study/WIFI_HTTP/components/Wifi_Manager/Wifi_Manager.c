#include "Wifi_Manager.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "stdio.h"

/*
    Wifi_Manager库回调函数:
        1. event_handler:
            WIFI和IP事件中断回调函数:只要有事件WIFI相关的函数调用就会触发,如connect函数,进行事件处理
        2. wifi_state_handler:(=wifi_callback)
            WIFI连接状态回调函数:WIFI或IP断开或者WIFI+IP都连接才会触发,只起通知作用,毕竟event_handler可以处理Any事件,当WIFI和IP都连接上时可以进行下一步处理了:MQTT/其他通信方式的连接,所以本库解耦度很高,只需要修改wifi_state_handler连接成功后选择哪种通信方式即可
*/

// 热点/WIFI - 账号,密码
#define DEFAULT_WIFI_SSID "lxl_WIFI"
#define DEFAULT_WIFI_PASSWORD "88888888"

// WIFI_Manager库的TAG
#define TAG "WIFI_Manager"

// WIFI连接状态回调函数
static p_wifi_state_cb wifi_callback = NULL ;

// WIFI重连
#define MAX_CONNECT_RETRY 10   // 最大重连次数
static int sta_connect_cnt = 0;//重连次数计数

//当前sta的ip连接状态
static bool is_sta_connected = false;

// ======================== WIFI和IP初始化函数 ========================

// WIFI和IP事件中断回调函数
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data) 
{
    // 先判断事件:WIFI还是IP
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect() ;
                sta_connect_cnt = 0 ;
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                if (is_sta_connected)
                {
                    is_sta_connected =  false ;
                    if (wifi_callback != NULL)
                    {
                        wifi_callback(WIFI_STATE_DISCONNECTED) ;
                    }
                }
                if (sta_connect_cnt < MAX_CONNECT_RETRY)
                {
                    esp_wifi_connect() ;
                    ESP_LOGI(TAG , "reconnect to AP") ;
                    sta_connect_cnt ++ ;
                    if (sta_connect_cnt == MAX_CONNECT_RETRY)
                    {
                        printf("\n\nPlease Reconnect WIFI\n\n") ;
                    }
                }
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG , "Connected to AP") ;
                break;
            default:    // 其他事件不处理
                break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        // 只关注ip是否连接成功
        if (event_id == IP_EVENT_STA_GOT_IP)
        {
            ESP_LOGI(TAG , "Got IP from AP");
            is_sta_connected = true ;
            if (wifi_callback != NULL)
            {
                wifi_callback(WIFI_STATE_CONNECTED) ;
            }
        }
    }
}

// WIFI和IP初始化
void Wifi_Manager_Init(p_wifi_state_cb f) 
{
    // 初始化nvs和网络接口
    ESP_ERROR_CHECK(nvs_flash_init());  // 掉电保存参数: 初始化nvs
    ESP_ERROR_CHECK(esp_netif_init());
    // 创建事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // 创建wifi station模式的网络接口
    esp_netif_create_default_wifi_sta();

    // 使用默认配置初始化wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();    // 默认配置
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,         // WiFi事件基声明,选择默认的宏定义即可
                                                        ESP_EVENT_ANY_ID,   // 任何事件ID都能触发事件中断处理
                                                        &event_handler,     // 事件中断函数
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,           
                                                        IP_EVENT_STA_GOT_IP,// station got IP from connected AP
                                                        &event_handler,     // 触发与上面相同的事件中断
                                                        NULL,
                                                        NULL));
    wifi_callback = f ;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) ); // Station模式

}

// WIFI配置密码和连接
void Wifi_Manager_Connect(const char *ssid , const char *password)
{
    wifi_config_t wifi_config = {
        .sta = {
            // authmode : The weakest authmode to accept in the fast scan mode
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,   // 加密方式,选择最简单的
        },
    };
    snprintf((char*)wifi_config.sta.ssid , 32 , "%s" , ssid) ;
    snprintf((char*)wifi_config.sta.password , 64 , "%s" , password) ;

    // 如果不是station模式，则设置为station模式，并重新启动wifi
    wifi_mode_t mode ;
    esp_wifi_get_mode(&mode);
    if (mode != WIFI_MODE_STA)
    {
        esp_wifi_stop() ;
        esp_wifi_set_mode(WIFI_MODE_STA) ;
    }
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config)) ;
    esp_wifi_start( );
}

// =========================== 外部(main)调用wifi功能 ===========================
// WIFI连接状态回调函数
void wifi_state_handler(WIFI_STATE state)
{
    switch (state)
    {
        case WIFI_STATE_CONNECTED:
            ESP_LOGI(TAG , "WIFI_STATE_CONNECTED");
            /* 开启协议连接 */
            
            break;
        case WIFI_STATE_DISCONNECTED:
            ESP_LOGI(TAG , "WIFI_STATE_DISCONNECTED");
            break;
        default:
            break;
    }
}

// WIFI初始化
void Wifi_Init(void)
{
    Wifi_Manager_Init(wifi_state_handler) ;
    Wifi_Manager_Connect(DEFAULT_WIFI_SSID , DEFAULT_WIFI_PASSWORD) ;
}

// WIFI断线重连(10次重连后需要再次调用该函数)
void Wifi_Reconnect(void)
{
    if (!is_sta_connected)
    {
        esp_wifi_connect() ;    // 要触发一下WIFI状态回调,激发重复WIFI重连
        ESP_LOGI(TAG , "reconnect to AP") ;
        sta_connect_cnt = 0 ;
    }
}
