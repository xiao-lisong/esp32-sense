set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_camera.c
    )

set(COMPONENT_REQUIRES 
    ecnu_log
    esp32-camera
    ecnu_packege
    ecnu_utils
    ecnu_transmit
)

register_component()