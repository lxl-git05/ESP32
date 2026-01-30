#ifndef __WIFI_WEBSOCKET_H
#define __WIFI_WEBSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <esp_http_server.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include <sys/stat.h>
#include "MyLED.h"
#include "esp_log.h"

// #include "connect_wifi.h"

// 处理HTTP GET请求,发送html
esp_err_t get_req_handler(httpd_req_t *req);
// 启动一个能同时处理网页和 WebSocket 消息的服务器
httpd_handle_t setup_websocket_server(void);
//
void wifi_http_main(void);


#endif
