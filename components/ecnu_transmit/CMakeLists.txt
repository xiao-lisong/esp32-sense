set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_transmit.c
    ecnu_mqtt.c
    )

set(COMPONENT_REQUIRES 
    ecnu_log
    mqtt
    )

register_component()