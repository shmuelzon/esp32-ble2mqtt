#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ble2mqtt

COMPONENT_ADD_INCLUDEDIRS := components/include

include $(IDF_PATH)/make/project.mk

MKSPIFFS=$(PROJECT_PATH)/mkspiffs/mkspiffs
SPIFFS_IMAGE=$(BUILD_DIR_BASE)/spiffs.bin

SERVICES=$(PROJECT_PATH)/data/services.json
CHARACTERISTICS=$(PROJECT_PATH)/data/characteristics.json

$(SERVICES) $(CHARACTERISTICS): get_gatt_assigned_numbers.py
	$(PYTHON) get_gatt_assigned_numbers.py

# Note: The $(shell ...) hack is to start a clean make session
# Clean mkspiffs
clean:
	rm -f $(SERVICES) $(CHARACTERISTICS)
	echo $(shell make -C $(PROJECT_PATH)/mkspiffs clean)

# Build mkspiffs utility
$(MKSPIFFS):
	echo $(shell make -C $(PROJECT_PATH)/mkspiffs)

# Build SPIFFS image
$(SPIFFS_IMAGE): $(PROJECT_PATH)/data $(SERVICES) $(CHARACTERISTICS) $(MKSPIFFS)
	$(MKSPIFFS) -c $< -b 4096 -p 256 -s 0x100000 $@

# Need to generate SPIFFS image before flashing
flash: $(SPIFFS_IMAGE)

# Include SPIFFS offset + image in the flash command
ESPTOOL_ALL_FLASH_ARGS += 0x110000 $(SPIFFS_IMAGE)
