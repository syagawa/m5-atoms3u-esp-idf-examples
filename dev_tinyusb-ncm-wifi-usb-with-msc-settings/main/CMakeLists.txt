idf_component_register(
    SRCS main.c
    INCLUDE_DIRS .
    PRIV_REQUIRES fatfs wear_levelling esp_partition esp_wifi nvs_flash
    REQUIRES json
)



# idf_component_register(
#     SRCS tusb_composite_main.c
#     INCLUDE_DIRS ../components
#     PRIV_REQUIRES fatfs wear_levelling esp_partition
# )

