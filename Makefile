#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ble2mqtt
PROJECT_VER := $(shell git describe --always --tags)
export PROJECT_VER

COMPONENT_ADD_INCLUDEDIRS := components/include

include $(IDF_PATH)/make/project.mk

# Build SPIFFS image
SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
$(eval $(call spiffs_create_partition_image,fs_0,data))
$(eval $(call spiffs_create_partition_image,fs_1,data))

OTA_TARGET ?= BLE2MQTT
OTA_FIRMWARE := $(BUILD_DIR_BASE)/$(PROJECT_NAME).bin
OTA_CONFIG := $(BUILD_DIR_BASE)/fs_0.bin
$(OTA_CONFIG): $(subst .bin,_bin,$(notdir $(OTA_CONFIG)))

check_project_python_requirements:
	$(CONFIG_SDK_PYTHON) \
	  $(IDF_PATH)/tools/check_python_dependencies.py -r requirements.txt

upload: $(OTA_FIRMWARE) | check_project_python_requirements
	echo Uploading firmware $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v $(PROJECT_VER) -t $(OTA_TARGET) -n Firmware

force-upload: $(OTA_FIRMWARE) | check_project_python_requirements
	echo Uploading firmware $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v \"\" -t $(OTA_TARGET) -n Firmware

upload-config: $(OTA_CONFIG) validate_config | check_project_python_requirements
	echo Uploading configuration $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v $(word 1, $(shell shasum -a 256 $<)) -t $(OTA_TARGET) -n Config

force-upload-config: $(OTA_CONFIG) validate_config | \
  check_project_python_requirements
	echo Uploading configuration $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v \"\" -t $(OTA_TARGET) -n Config

remote-monitor:
	$(CONFIG_PYTHON) -u $(PROJECT_PATH)/remote_log.py

validate_config: $(PROJECT_PATH)/data/config.json
	cat $< | $(CONFIG_PYTHON) -m json.tool > /dev/null || \
	  (echo "Error: Invalid JSON in configuration file."; exit 1)

.PHONY: upload force-upload upload-config force-upload-config remote-monitor \
  validate_config check_project_python_requirements
