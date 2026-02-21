#include "ADC.h"
#include "ADC_App.h"

// ====================== 外部任务 ======================
// 光强传感器
void ADC_Gray_Init(void)
{
    adc_init(Gray_Channel) ;
}

void ADC_Get_Gray(int *Gray_Data)
{
    ADC_Read_Raw(Gray_Data , Gray_Channel) ;
}

// 4按键传感器
void ADC_Key_Init(void)
{
    adc_init(Gray_Channel) ;
}

void ADC_Get_Key(int *Gray_Data)
{
    ADC_Read_Raw(Gray_Data , Gray_Channel) ;
}
