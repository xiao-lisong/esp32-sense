set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_SRCS 
    ecnu_packege_encoder.c
    )

set(COMPONENT_REQUIRES 
    json
    ecnu_log
)

register_component()