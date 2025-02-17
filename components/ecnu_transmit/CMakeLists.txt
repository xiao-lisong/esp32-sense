set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_transmit.c
    ecnu_mqtt.c
    ecnu_tcp.c
    )

set(COMPONENT_REQUIRES 
    ecnu_log
    ecnu_config
    mqtt
    )

register_component()