idf_component_register(
    # SRCS main.c
    # INCLUDE_DIRS .
    SRCS "../hid_device/usb_descriptors.c" "main.c"
    INCLUDE_DIRS "../hid_device ."
    PRIV_REQUIRES fatfs wear_levelling esp_partition
    REQUIRES json tinyusb
)


if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
idf_component_get_property(tusb_lib espressif__tinyusb COMPONENT_LIB)
else()
idf_component_get_property(tusb_lib leeebo__tinyusb_src COMPONENT_LIB)
endif()

cmake_policy(SET CMP0079 NEW)
target_link_libraries(${tusb_lib} PRIVATE ${COMPONENT_LIB})
# idf_component_register(
#     SRCS tusb_composite_main.c
#     INCLUDE_DIRS ../components
#     PRIV_REQUIRES fatfs wear_levelling esp_partition
# )

