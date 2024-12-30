set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_wifi.c
    )

set(COMPONENT_REQUIRES 
    ecnu_log
    esp_wifi
    )

register_component()