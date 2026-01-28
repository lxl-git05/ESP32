#ifndef __WIFI_MANAGER_H
#define __WIFI_MANAGER_H

// wifi状态
typedef enum
{
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
}WIFI_STATE;

// 回调函数,通知wifi状态
typedef void (*p_wifi_state_cb)(WIFI_STATE) ;
// WIFI初始化
void Wifi_Manager_Init(p_wifi_state_cb f) ;
// WIFI配置密码和连接
void Wifi_Manager_Connect(const char *ssid , const char *password) ;
#endif
