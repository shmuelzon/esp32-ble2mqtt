idf_component_register(
    SRCS "ble.c" "ble2mqtt.c" "ble_utils.c" "broadcasters.c" "config.c"
        "eth.c" "gatt.c" "httpd.c" "log.c" "mqtt.c" "ota.c" "resolve.c" "wifi.c"
    INCLUDE_DIRS ".")

target_compile_definitions(${COMPONENT_TARGET} PRIVATE
    "-DBLE2MQTT_VER=\"${PROJECT_VER}\"")
target_include_directories(${COMPONENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Static files to be served by the web server
file(GLOB_RECURSE www_files RELATIVE ${PROJECT_DIR}/www ${PROJECT_DIR}/www/*)
foreach(file ${www_files})
    string(MAKE_C_IDENTIFIER ${file} c_name)
    string(APPEND static_files_list
        "{ \"/${file}\", ${c_name}_start, ${c_name}_end },\n    ")
    string(APPEND static_files_declerations
        "extern const char ${c_name}_start[] asm(\"_binary_${c_name}_start\");\n")
    string(APPEND static_files_declerations
        "extern const char ${c_name}_end[] asm(\"_binary_${c_name}_end\");\n")
    get_filename_component(www_file_dir ${build_dir}/www/${file}.gz DIRECTORY)
    file(MAKE_DIRECTORY ${www_file_dir})
    add_custom_command(OUTPUT ${build_dir}/www/${file}.gz
        COMMAND tar -czf ${build_dir}/www/${file}.gz ${PROJECT_DIR}/www/${file}
        DEPENDS ${PROJECT_DIR}/www/${file})
    target_add_binary_data(${COMPONENT_LIB} ${build_dir}/www/${file}.gz
        BINARY RENAME_TO ${c_name})
endforeach()
configure_file(httpd_static_files.h.in httpd_static_files.h)
