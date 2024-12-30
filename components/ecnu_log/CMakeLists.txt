set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_log.c
    )

set(COMPONENT_REQUIRES esp_wifi)

register_component()