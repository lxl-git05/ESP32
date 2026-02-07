#include "Wifi_Http.h"

#include "string.h"
#include <stdlib.h>
#include "stdio.h"

#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"

#include "Wifi_Http_HTML.h" // 内含自设HTML界面

extern float hum  ;
extern float temp ;

// ========================= GET请求处理 =========================
// 更新数据 , 发送web页面
esp_err_t send_web_page(httpd_req_t *req)   // *req : 
{
    // 更新想要呈现的数据

    // 构建插入数据到HTML原本的占位符里
    char response_data[sizeof(html_page) + 50];
    memset(response_data, 0, sizeof(response_data));
    sprintf(response_data, html_page, temp, hum);

    // 发送应答
    esp_err_t err = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN) ;
    ESP_ERROR_CHECK(err);
    return err;
}

// 应答GET请求回调函数
esp_err_t get_req_handler(httpd_req_t *req)
{
    esp_err_t err = send_web_page(req);
    ESP_ERROR_CHECK(err);
    return err;
}

// ========================= POST请求处理 =========================

// 暂时略

// ========================= 初始化 =========================
// 初始化HTTP协议
httpd_handle_t Http_Init(void)
{
    // 配置结构体
    httpd_config_t config = HTTPD_DEFAULT_CONFIG() ;    // 使用默认的参数
    httpd_handle_t server = NULL ;                      // 服务句柄

    // 处理web端的Get请求
    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,             // 这表明web在请求HTML页面
        .handler = get_req_handler,     // 当匹配到 uri + method 时，httpd 会调用这个函数
        .user_ctx = NULL
    };

    if (httpd_start(&server , &config) == ESP_OK)
    {
        httpd_register_uri_handler(server , &uri_get) ;
    }

    // 返回http注册状态
    return server ;
}
