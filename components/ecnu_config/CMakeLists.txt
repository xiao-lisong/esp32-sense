set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_config.c
    )

set(COMPONENT_REQUIRES ecnu_log nvs_flash json)

register_component()