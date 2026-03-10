idf_component_register(SRCS "MPU6050_Angle.c" "MPU6050.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver system_I2C Msg tool_Timer_Counter)
