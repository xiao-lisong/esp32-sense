set(requires
    nvs_flash
    ecnu_log
    ecnu_wifi
    ecnu_transmit
    ecnu_data
    ecnu_config
    )

set(APP_SRCS
    app_main.c
    )

idf_component_register(SRCS ${APP_SRCS}
                       REQUIRES ${requires})

