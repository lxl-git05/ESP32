#ifndef __CAM_H
#define __CAM_H

#include <esp_log.h>
#include "esp_err.h"
#include "esp_http_server.h"

// 初始化CAM,记得开PSRAM
esp_err_t Cam_Init(void) ;

// 实验:拍照
void picture(void);

// 注册图传URI
esp_err_t start_pic_server(httpd_handle_t* handle , httpd_config_t* config) ;
#endif
