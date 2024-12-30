set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_data.c
    )

set(COMPONENT_REQUIRES 
    ecnu_camera
)

register_component()