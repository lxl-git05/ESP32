#ifndef __WIFI_MANAGER_H
#define __WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_wifi.h"

// wifi状态
typedef enum
{
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
}WIFI_STATE;

// 回调函数,通知wifi状态
typedef void (*p_wifi_state_cb)(WIFI_STATE) ;
// 回调函数,通知扫描到的wifi信息
typedef void (*p_wifi_scan_cb)(int num , wifi_ap_record_t *ap_records) ;

// WIFI初始化
void Wifi_Manager_Init(p_wifi_state_cb f) ;
// AP配网
esp_err_t Wifi_Maneger_ap(void) ;
// WIFI配置密码和连接
void Wifi_Manager_Connect(const char *ssid , const char *password) ;
// AP扫描模式,扫描附近的热点
esp_err_t Wifi_Manager_scan(p_wifi_scan_cb f) ;
#endif
