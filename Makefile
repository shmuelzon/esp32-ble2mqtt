#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ble2mqtt
PROJECT_VER := $(shell git describe --always --tags)
export PROJECT_VER

COMPONENT_ADD_INCLUDEDIRS := components/include

include $(IDF_PATH)/make/project.mk

MKSPIFFS=$(PROJECT_PATH)/mkspiffs/mkspiffs
SPIFFS_IMAGE=$(BUILD_DIR_BASE)/spiffs.bin

# Note: The $(shell ...) hack is to start a clean make session
# Clean mkspiffs
clean: clean_spiffs

clean_spiffs:
	echo $(shell make -C $(PROJECT_PATH)/mkspiffs clean)

# Build mkspiffs utility
$(MKSPIFFS):
	echo $(shell make -C $(PROJECT_PATH)/mkspiffs)

SPIFFS_PARTITION=$(shell grep "^storage" partitions.csv | sed 's/,//g')
SPIFFS_OFFSET=$(word 4, $(SPIFFS_PARTITION))
SPIFFS_SIZE=$(word 5, $(SPIFFS_PARTITION))

# Build SPIFFS image
$(SPIFFS_IMAGE): $(PROJECT_PATH)/data $(MKSPIFFS) partitions.csv
	$(MKSPIFFS) -c $< -b 4096 -p 256 -s $(SPIFFS_SIZE) $@

# Need to generate SPIFFS image before flashing
flash: $(SPIFFS_IMAGE)

# Include SPIFFS offset + image in the flash command
ESPTOOL_ALL_FLASH_ARGS += $(SPIFFS_OFFSET) $(SPIFFS_IMAGE)

OTA_TARGET ?= BLE2MQTT
OTA_FIRMWARE := $(BUILD_DIR_BASE)/$(PROJECT_NAME).bin
OTA_CONFIG := $(PROJECT_PATH)/data/config.json
MD5 := $(if $(subst Darwin,,$(shell uname)),md5sum,md5 -r)

upload: $(OTA_FIRMWARE)
	echo Uploading firmware $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v $(PROJECT_VER) -t $(OTA_TARGET) -n Firmware

force-upload: $(OTA_FIRMWARE)
	echo Uploading firmware $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v \"\" -t $(OTA_TARGET) -n Firmware

upload-config: $(OTA_CONFIG)
	echo Uploading configuration $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v $(word 1, $(shell $(MD5) $<)) -t $(OTA_TARGET) -n Config

force-upload-config: $(OTA_CONFIG)
	echo Uploading configuration $< to $(OTA_TARGET)
	$(CONFIG_PYTHON) $(PROJECT_PATH)/ota.py -f $< \
	  -v \"\" -t $(OTA_TARGET) -n Config

remote-monitor:
	$(CONFIG_PYTHON) $(PROJECT_PATH)/remote_log.py

.PHONY: upload force-upload upload-config force-upload-config remote-monitor
