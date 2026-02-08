#ifndef __CAM_H
#define __CAM_H

#include <esp_log.h>
#include "esp_err.h"

// 初始化CAM,记得开PSRAM
esp_err_t Cam_Init(void) ;

// 实验:拍照
void picture(void);

#endif
