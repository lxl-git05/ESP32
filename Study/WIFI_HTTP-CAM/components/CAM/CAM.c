#include "CAM.h"

#include "esp_camera.h"
#include "camera_pin.h"

#include <esp_system.h>
#include <string.h>
#include "stdio.h"

#define TAG "CAM"
#define CONFIG_XCLK_FREQ 20000000 

// 根据摄像头进行自适应
static void camera_sensor_adjust(void)
{
    // 得到摄像头型号,读取传感器ID
    sensor_t *s = esp_camera_sensor_get();  
    s->set_vflip(s, 1); // 翻转摄像头
    ESP_LOGI("CAM", "PID: 0x%04X", s->id.PID);  // 打印型号
    // 根据摄像头型号调整初始化参数
    if (s->id.PID == OV3660_PID) {
        s->set_brightness(s, 1); // up the blightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }

    if (s->id.PID == OV3660_PID || s->id.PID == OV2640_PID) {
        s->set_vflip(s, 1); // flip it back
    } else if (s->id.PID == GC0308_PID) {
        s->set_hmirror(s, 0);
    } else if (s->id.PID == GC032A_PID) {
        s->set_vflip(s, 1);
    }

    // Get the basic information of the sensor.
    camera_sensor_info_t *s_info = esp_camera_sensor_get_info(&(s->id));

    if (s_info->support_jpeg != true) {
        printf("NO Jpeg Support!\n") ;
    }
}

// 初始化CAM,记得开PSRAM
esp_err_t Cam_Init(void)
{
    camera_config_t camcfg = {
        .pin_pwdn = CAM_PIN_PWDN,       // -1
        .pin_reset = CAM_PIN_RESET,     // -1

        .pin_xclk = CAM_PIN_XCLK,       
        .pin_sscb_sda = CAM_PIN_SIOD,
        .pin_sscb_scl = CAM_PIN_SIOC,

        .pin_d7 = CAM_PIN_D7,
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,

        // EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode.
        .xclk_freq_hz = CONFIG_XCLK_FREQ,
        .ledc_timer = LEDC_TIMER_0, // This is only valid on ESP32/ESP32-S2. ESP32-S3 use LCD_CAM interface.
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_JPEG,  // 图像格式
        .frame_size = FRAMESIZE_QVGA,    // 图像大小

        .jpeg_quality = 30, // 0-63 lower means higher quality
        .fb_count = 1,       // For ESP32/ESP32-S2, if more than one, i2s runs in continuous mode.
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
        .fb_location = CAMERA_FB_IN_PSRAM
    };

    esp_err_t err = esp_camera_init(&camcfg);
    ESP_ERROR_CHECK(err) ;

    // 摄像头参数初始化
    camera_sensor_adjust() ;

    return ESP_OK;
}

// 实验:拍照
void picture(void)
{
    ESP_LOGI(TAG, "Taking picture...");
    camera_fb_t *pic = esp_camera_fb_get();

    if (pic) {
        // use pic->buf to access the image
        ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
        // To enable the frame buffer can be reused again.
        // Note: If you don't call fb_return(), the next time you call fb_get() you may get
        // an error "Failed to get the frame on time!"because there is no frame buffer space available.
        esp_camera_fb_return(pic);
    }
}
