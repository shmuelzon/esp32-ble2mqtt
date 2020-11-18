idf_component_register(
    SRCS "ble.c" "ble2mqtt.c" "ble_utils.c" "broadcasters.c" "config.c"
        "eth.c" "gatt.c" "log.c" "mqtt.c" "ota.c" "resolve.c" "wifi.c"
    INCLUDE_DIRS ".")

target_compile_definitions(${COMPONENT_TARGET} PRIVATE
    "-DBLE2MQTT_VER=\"${PROJECT_VER}\"")
