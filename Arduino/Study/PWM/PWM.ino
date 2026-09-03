// 项目: PWM.ino
// 
void setup() 
{
    // 串口初始化
    Serial.begin(115200);
    // 设置 PWM 引脚为输出模式
    pinMode(LED_BUILTIN, OUTPUT);
    // 设置 PWM 占空比为 50%
    analogWrite(LED_BUILTIN, 127); // 127 是 50% 的占空比 (0-255)
}

void loop() 
{
    // 串口控制PWM
    if (Serial.available() > 0) 
    {
        int value = Serial.parseInt(); // 从串口读取整数值
        if (value >= 0 && value <= 255) 
        {
            analogWrite(LED_BUILTIN, value); // 设置 PWM 占空比
        }
        Serial.printf("PWM Value Set To: %d", value); // 输出当前设置的 PWM 值
    }
}
