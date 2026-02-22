idf_component_register(SRCS "Buzzer.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver Msg system_PWM)
