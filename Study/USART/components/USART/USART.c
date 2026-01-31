#include "USART.h"

#define Debug_USART
#define TAG_TX "USART_TX"
#define TAG_RX "USART_RX"

// 初始化
void USART_Init(void)
{
    // 配置参数
    uart_config_t uart_config = {
        .baud_rate = 115200 ,
        .data_bits = UART_DATA_8_BITS ,
        // .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS ,
        .parity = UART_PARITY_DISABLE ,
        .rx_flow_ctrl_thresh = 122 ,
        .stop_bits = UART_STOP_BITS_1 ,
        .source_clk = UART_SCLK_DEFAULT ,
        // 如果 RX 信号可能出现抖动，可以设置 uart_config_t::rx_glitch_filt_thresh 来过滤抖动以确保接收到正确的数据
    };
    ESP_ERROR_CHECK(uart_param_config(USART_PORT_0,&uart_config)) ;
    // 配置通信管脚
    ESP_ERROR_CHECK(uart_set_pin(USART_PORT_0, USART_TX_PIN, USART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // 安装驱动程序
    QueueHandle_t uart_queue ;
    ESP_ERROR_CHECK(uart_driver_install(USART_PORT_0, USART_BUFFER_SIZE, USART_BUFFER_SIZE, 10, &uart_queue, 0) );
    
}

// 发送消息
int sendData(const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(USART_PORT_0, data, len);
    ESP_LOGI(TAG_TX, "Wrote %d bytes", txBytes);
    return txBytes;
}

// 接收数据:阻塞式
void ReceiveData_Zuse(char recData[])
{
    // 超时时间为100ms
    int size = uart_read_bytes(USART_PORT_0 , recData , 128 , pdMS_TO_TICKS(100)) ;
    if (size > 0)
    {
        recData[size] = '\0' ;
        #ifdef Debug_USART
        printf("Receive : %s\n" , recData ) ;
        #endif
    }
}

