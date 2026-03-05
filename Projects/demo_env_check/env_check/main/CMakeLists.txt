idf_component_register(SRCS "model_data.cc" "ai_model.cpp" "MyTask.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES espressif__esp-tflite-micro Msg MyLED Key_4 Gray MPU6050 OLED DHT11 RGB_LED Buzzer  system_ADC  system_I2C  system_PWM  system_Timer  tool_Timer_Counter)
