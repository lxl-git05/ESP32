# 通过idf_component_register函数注册组件，相当于实现了自己的自建库函数，记得SRCS后面改名哦.c
idf_component_register(
    SRCS "MyLED.c"
    INCLUDE_DIRS "."
    REQUIRES driver
)# 这里还要进行组件依赖(main函数不需要,因为做了特殊处理，但是其他自设库需要进行这个操作)
# 组件依赖还有传递性,比如MyLED依赖esp_driver_gpio,那么esp_driver_gpio也需要依赖其他组件，比如esp_system,那么esp_system也需要依赖其他组件，以此类推
# 所以在idf_component_register函数中，REQUIRES后面可以写多个组件名，用空格隔开，依赖关系会自动递归处理

# 还有很多组件被存在了云端
# 自己去官网找,然后指令执行