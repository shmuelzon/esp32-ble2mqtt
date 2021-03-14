cmake_minimum_required(VERSION 3.5)

execute_process(
    COMMAND git describe --always --tags
    OUTPUT_VARIABLE PROJECT_VER
    OUTPUT_STRIP_TRAILING_WHITESPACE)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
add_compile_definitions(BTA_GATTC_NOTIF_REG_MAX=32)

project(ble2mqtt)

spiffs_create_partition_image(fs_0 data FLASH_IN_PROJECT)
spiffs_create_partition_image(fs_1 data FLASH_IN_PROJECT)
add_dependencies(spiffs_fs_0_bin validate-config)

add_custom_target(check-project-python-requirements
    COMMAND ${python} $ENV{IDF_PATH}/tools/check_python_dependencies.py
        -r ${PROJECT_DIR}/requirements.txt)

if(CMAKE_HOST_WIN32)
    set(NULDEV NUL)
else()
    set(NULDEV /dev/null)
endif()

add_custom_target(validate-config
    COMMAND ${python} -m json.tool ${PROJECT_DIR}/data/config.json >${NULDEV}
        || (echo "Error: Invalid JSON in configuration file." && exit 1 ))

add_custom_target(upload
    COMMAND ${python} ${PROJECT_DIR}/ota.py -f ${build_dir}/${PROJECT_BIN}
        -v ${PROJECT_VER} -t $$\{OTA_TARGET:-BLE2MQTT\} -n Firmware
    DEPENDS check-project-python-requirements app validate-config
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_DIR})

add_custom_target(force-upload
    COMMAND ${python} ${PROJECT_DIR}/ota.py -f ${build_dir}/${PROJECT_BIN}
        -v \"\" -t $$\{OTA_TARGET:-BLE2MQTT\} -n Firmware
    DEPENDS check-project-python-requirements app validate-config
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_DIR})

add_custom_target(upload-config
    COMMAND ${python} ${PROJECT_DIR}/ota.py -f ${build_dir}/fs_0.bin
        -v $$\(shasum -a 256 ${build_dir}/fs_0.bin | awk '{ print $$1 }'\)
        -t $$\{OTA_TARGET:-BLE2MQTT\} -n Config
    DEPENDS check-project-python-requirements spiffs_fs_0_bin validate-config
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_DIR})

add_custom_target(force-upload-config
    COMMAND ${python} ${PROJECT_DIR}/ota.py -f ${build_dir}/fs_0.bin -v \"\"
        -t $$\{OTA_TARGET:-BLE2MQTT\} -n Config
    DEPENDS check-project-python-requirements spiffs_fs_0_bin validate-config
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_DIR})

add_custom_target(remote-monitor
    COMMAND ${python} -u ${PROJECT_DIR}/remote_log.py
    DEPENDS validate-config
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_DIR})

add_custom_target(image
    COMMAND esptool.py --chip esp32 merge_bin -o image.bin @flash_project_args
    DEPENDS app)
