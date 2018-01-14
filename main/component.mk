#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

GATT_C := $(COMPONENT_PATH)/gatt.c
GATT_H := $(COMPONENT_PATH)/gatt.h

COMPONENT_EXTRA_CLEAN := $(GATT_C) $(GATT_H)

config.o: $(GATT_H)

$(GATT_C) $(GATT_H): $(PROJECT_PATH)/get_gatt_assigned_numbers.py
	$(CONFIG_PYTHON) $(PROJECT_PATH)/get_gatt_assigned_numbers.py \
	  -H $(GATT_H) -C $(GATT_C)
