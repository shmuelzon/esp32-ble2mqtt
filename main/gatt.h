#ifndef GATT_H
#define GATT_H

#include "ble_utils.h"

typedef enum {
    CHAR_TYPE_UNKNOWN,
    CHAR_TYPE_16BIT,
    CHAR_TYPE_24BIT,
    CHAR_TYPE_2BIT,
    CHAR_TYPE_32BIT,
    CHAR_TYPE_4BIT,
    CHAR_TYPE_8BIT,
    CHAR_TYPE_BOOLEAN,
    CHAR_TYPE_FLOAT,
    CHAR_TYPE_FLOAT64,
    CHAR_TYPE_GATT_UUID,
    CHAR_TYPE_NIBBLE,
    CHAR_TYPE_REG_CERT_DATA_LIST,
    CHAR_TYPE_SFLOAT,
    CHAR_TYPE_SINT16,
    CHAR_TYPE_SINT24,
    CHAR_TYPE_SINT32,
    CHAR_TYPE_SINT8,
    CHAR_TYPE_UINT12,
    CHAR_TYPE_UINT128,
    CHAR_TYPE_UINT16,
    CHAR_TYPE_UINT24,
    CHAR_TYPE_UINT32,
    CHAR_TYPE_UINT40,
    CHAR_TYPE_UINT48,
    CHAR_TYPE_UINT8,
    CHAR_TYPE_UTF8S,
    CHAR_TYPE_VARIABLE
} characteristic_type_t;

typedef struct {
    ble_uuid_t uuid;
    char *name;
    characteristic_type_t *types;
} characteristic_desc_t;

typedef struct {
    ble_uuid_t uuid;
    char *name;
} service_desc_t;

extern service_desc_t services[];
extern characteristic_desc_t characteristics[];

#endif