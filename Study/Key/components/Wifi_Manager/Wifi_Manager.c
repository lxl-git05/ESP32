#include "Wifi_Manager.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "stdio.h"
#include "lwip/ip4_addr.h"

static p_wifi_state_cb wifi_callback = NULL ;

#define MAX_CONNECT_RETRY 10
static int sta_connect_cnt = 0;//重连的次数

//当前sta的ip连接状态
static bool is_sta_connected = false;

// AP配网参数
static const char *ap_ssid_name = "ESP32-AP" ;
static const char *ap_password = "12345678" ;
static esp_netif_t* esp_netif_ap = NULL ;
static SemaphoreHandle_t scan_sem = NULL;

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
                    ESP_LOGI("WIFI_Disconnect" , "reconnect to AP") ;
                    sta_connect_cnt ++ ;
                }
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI("WIFI_Connected" , "Connected to AP") ;
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI("WIFI_AP_STACONNECTED" , "A station connected") ;
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI("WIFI_AP_STADISCONNECTED" , "A station disconnected") ;
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
            ESP_LOGI("WIFI_Got_IP" , "Got IP from AP");
            is_sta_connected = true ;
            if (wifi_callback != NULL)
            {
                wifi_callback(WIFI_STATE_CONNECTED) ;
            }
        }
    }
}

// WIFI初始化
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
    esp_netif_ap = esp_netif_create_default_wifi_ap() ;     // AP配网参数
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
    scan_sem = xSemaphoreCreateBinary() ;
    xSemaphoreGive(scan_sem) ;
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

// AP配网
esp_err_t Wifi_Maneger_ap(void)
{
    wifi_mode_t mode ;
    esp_wifi_get_mode(&mode);

    // 检查是否进入AP + STA模式,必须有后者,否则无法扫描
    if (mode == WIFI_MODE_APSTA)
    {
        return ESP_OK ;
    }
    // 断线重连
    esp_wifi_disconnect() ;
    esp_wifi_stop() ;
    esp_wifi_set_mode(WIFI_MODE_APSTA) ;
    // 配置参数
    wifi_config_t wifi_config = {
        .ap = {
            .channel = 5 ,  // AP的信道,随便给的
            .max_connection = 2 ,   // 最大连接数
            .authmode = WIFI_AUTH_WPA_WPA2_PSK ,   // WPA2加密
        }
    };
    sprintf((char*)wifi_config.ap.ssid , 32 , "%s" , ap_ssid_name) ;
    wifi_config.ap.ssid_len = strlen(ap_ssid_name) ;
    sprintf((char*)wifi_config.ap.password , 64 , "%s" , ap_password) ;
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config) ;

    // IP,网关,子网掩码
    esp_netif_ip_info_t ip_info ;
    IP4_ADDR(&ip_info.ip , 192 , 168 , 100 , 1) ;
    IP4_ADDR(&ip_info.gw , 192 , 168 , 100 , 1) ;
    IP4_ADDR(&ip_info.netmask , 255 , 255 , 255 , 0) ;

    esp_netif_dhcps_stop(&esp_netif_ap) ;   // 先关闭DHCP服务,原因见esp_netif_set_ip_info注释
    esp_netif_set_ip_info(esp_netif_ap , &ip_info) ;    
    esp_netif_dhcps_start(&esp_netif_ap) ;

    return esp_wifi_start() ;
}

static void scan_task(void *param)
{
    p_wifi_scan_cb callback = (p_wifi_scan_cb)param ;
    uint16_t ap_count = 0 ;
    uint16_t ap_num = 20 ;
    wifi_ap_record_t ap_list[20] ;

    esp_wifi_scan_start(NULL , true) ;
    esp_wifi_scan_get_ap_num(&ap_count) ;
    esp_wifi_scan_get_ap_records(&ap_num , ap_list) ;

    ESP_LOGI("WIFI_Scan" , "Total AP Count: %d Actual Num: %d" , ap_count ,ap_num) ;
    if (callback != NULL)
    {
        callback(ap_num , ap_list) ;
    }
    xSemaphoreGive(scan_sem) ;
    vTaskDelete(NULL) ;
}

// 扫描模式
esp_err_t Wifi_Manager_scan(p_wifi_scan_cb f)
{
    if (pdTRUE == xSemaphoreTake(scan_sem , 0))
    {
        esp_wifi_clear_ap_list() ;
        return xTaskCreatePinnedToCore(scan_task , "scan_task" , 4096 , f , 3 , NULL , 1) ;
    }
    return ESP_OK ;
}