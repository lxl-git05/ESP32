#include "ADC.h"
#include "esp_err.h"
#include "stdbool.h"
adc_oneshot_unit_handle_t adc_handle;

// ADC初始化
void adc_init(adc_channel_t adc_channel)
{
    // 初始化只调用一次
    static bool is_adc_unit_init = false ;
    if (is_adc_unit_init == false)
    {
        adc_oneshot_unit_init_cfg_t init_config = {
            .unit_id = ADC_UNIT_1,  // 不建议使用ADC2
            .ulp_mode = ADC_ULP_MODE_DISABLE ,
        };

        esp_err_t ret = adc_oneshot_new_unit(&init_config, &adc_handle) ;
        ESP_ERROR_CHECK(ret) ;
        is_adc_unit_init = true ;
    }

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,   // 0~3.3V
        .bitwidth = ADC_BITWIDTH_12,// 12位
    };

    // 创建通道,这个可以复写
    adc_oneshot_config_channel(adc_handle, adc_channel,&config); 
}

// 读取raw值
void  ADC_Read_Raw(int *ADC_Data , adc_channel_t adc_Channel)
{
    esp_err_t err ;
    err = adc_oneshot_read(adc_handle , adc_Channel , ADC_Data) ;
    ESP_ERROR_CHECK(err) ;
}

