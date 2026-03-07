#include "ADC.h"

#include "esp_err.h"

adc_oneshot_unit_handle_t adc_handle;

// ADC初始化
void adc_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,  // 不建议使用ADC2
        .ulp_mode = ADC_ULP_MODE_DISABLE ,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle)) ;

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,   // 0~3.3V
        .bitwidth = ADC_BITWIDTH_12,// 12位
    };

    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_8,&config); 
}

// 读取raw值
void  ADC_Read_Raw(int *ADC_Data)
{
    esp_err_t err ;
    err = adc_oneshot_read(adc_handle , ADC_Channel , ADC_Data) ;
    ESP_ERROR_CHECK(err) ;
}
