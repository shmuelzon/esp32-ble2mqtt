#include "gatt.h"
#include <stdlib.h>

service_desc_t services[] = {
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00 },
        .name = "GenericAccess",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x01, 0x18, 0x00, 0x00 },
        .name = "GenericAttribute",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x02, 0x18, 0x00, 0x00 },
        .name = "ImmediateAlert",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x03, 0x18, 0x00, 0x00 },
        .name = "LinkLoss",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x04, 0x18, 0x00, 0x00 },
        .name = "TxPower",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x05, 0x18, 0x00, 0x00 },
        .name = "CurrentTimeService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x06, 0x18, 0x00, 0x00 },
        .name = "ReferenceTimeUpdateService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x07, 0x18, 0x00, 0x00 },
        .name = "NextDSTChangeService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x08, 0x18, 0x00, 0x00 },
        .name = "Glucose",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x09, 0x18, 0x00, 0x00 },
        .name = "HealthThermometer",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x18, 0x00, 0x00 },
        .name = "DeviceInformation",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0d, 0x18, 0x00, 0x00 },
        .name = "HeartRate",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0e, 0x18, 0x00, 0x00 },
        .name = "PhoneAlertStatusService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0f, 0x18, 0x00, 0x00 },
        .name = "BatteryService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x10, 0x18, 0x00, 0x00 },
        .name = "BloodPressure",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0x18, 0x00, 0x00 },
        .name = "AlertNotificationService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00 },
        .name = "HumanInterfaceDevice",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x13, 0x18, 0x00, 0x00 },
        .name = "ScanParameters",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x14, 0x18, 0x00, 0x00 },
        .name = "RunningSpeedandCadence",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x15, 0x18, 0x00, 0x00 },
        .name = "AutomationIO",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x16, 0x18, 0x00, 0x00 },
        .name = "CyclingSpeedandCadence",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00 },
        .name = "CyclingPower",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x18, 0x00, 0x00 },
        .name = "LocationandNavigation",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1a, 0x18, 0x00, 0x00 },
        .name = "EnvironmentalSensing",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1b, 0x18, 0x00, 0x00 },
        .name = "BodyComposition",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1c, 0x18, 0x00, 0x00 },
        .name = "UserData",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1d, 0x18, 0x00, 0x00 },
        .name = "WeightScale",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1e, 0x18, 0x00, 0x00 },
        .name = "BondManagementService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1f, 0x18, 0x00, 0x00 },
        .name = "ContinuousGlucoseMonitoring",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x20, 0x18, 0x00, 0x00 },
        .name = "InternetProtocolSupportService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x21, 0x18, 0x00, 0x00 },
        .name = "IndoorPositioning",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x22, 0x18, 0x00, 0x00 },
        .name = "PulseOximeterService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x23, 0x18, 0x00, 0x00 },
        .name = "HTTPProxy",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x24, 0x18, 0x00, 0x00 },
        .name = "TransportDiscovery",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x25, 0x18, 0x00, 0x00 },
        .name = "ObjectTransferService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x26, 0x18, 0x00, 0x00 },
        .name = "FitnessMachine",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x27, 0x18, 0x00, 0x00 },
        .name = "MeshProvisioningService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x28, 0x18, 0x00, 0x00 },
        .name = "MeshProxyService",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x29, 0x18, 0x00, 0x00 },
        .name = "ReconnectionConfiguration",
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3a, 0x18, 0x00, 0x00 },
        .name = "InsulinDelivery",
    },
    {}
};

static characteristic_type_t types_00002a00_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a01_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a02_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_BOOLEAN, -1 };
static characteristic_type_t types_00002a03_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT48, -1 };
static characteristic_type_t types_00002a04_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a05_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a06_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a07_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT8, -1 };
static characteristic_type_t types_00002a08_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a09_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a0a_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a0b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a0c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a0d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a0e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT8, -1 };
static characteristic_type_t types_00002a0f_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a10_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a11_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a12_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a13_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a14_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a15_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a16_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a17_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a18_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_NIBBLE, CHAR_TYPE_NIBBLE, CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a19_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a1a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a1b_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a1c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_FLOAT, CHAR_TYPE_FLOAT, -1 };
static characteristic_type_t types_00002a1d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a1e_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a1f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002a20_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002a21_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a22_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a23_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT40, CHAR_TYPE_UINT24, -1 };
static characteristic_type_t types_00002a24_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a25_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a26_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a27_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a28_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a29_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a2a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_REG_CERT_DATA_LIST, -1 };
static characteristic_type_t types_00002a2b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a2c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a2f_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a30_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a31_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a32_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a33_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a34_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT16, CHAR_TYPE_8BIT, CHAR_TYPE_UINT8, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT8, CHAR_TYPE_NIBBLE, CHAR_TYPE_NIBBLE, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, -1 };
static characteristic_type_t types_00002a35_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT8, CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a36_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a37_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a38_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a39_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a3a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a3b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a3c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_FLOAT64, -1 };
static characteristic_type_t types_00002a3d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a3e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a3f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a40_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a41_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a42_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a43_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a44_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a45_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a46_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a47_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a48_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a49_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a4a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_8BIT, CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002a4b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a4c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a4d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a4e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a4f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a50_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a51_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a52_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, -1 };
static characteristic_type_t types_00002a53_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT32, -1 };
static characteristic_type_t types_00002a54_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a55_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, -1 };
static characteristic_type_t types_00002a56_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_2BIT, -1 };
static characteristic_type_t types_00002a57_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_2BIT, -1 };
static characteristic_type_t types_00002a58_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a59_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a5a_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a5b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT32, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a5c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, -1 };
static characteristic_type_t types_00002a5d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a5e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_16BIT, CHAR_TYPE_24BIT, CHAR_TYPE_SFLOAT, -1 };
static characteristic_type_t types_00002a5f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_16BIT, CHAR_TYPE_24BIT, CHAR_TYPE_SFLOAT, -1 };
static characteristic_type_t types_00002a60_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_16BIT, CHAR_TYPE_24BIT, -1 };
static characteristic_type_t types_00002a62_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_8BIT, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a63_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_SINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT32, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT12, CHAR_TYPE_UINT12, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a64_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002a65_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_32BIT, -1 };
static characteristic_type_t types_00002a66_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, -1 };
static characteristic_type_t types_00002a67_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT24, CHAR_TYPE_SINT32, CHAR_TYPE_SINT32, CHAR_TYPE_SINT24, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a68_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT24, CHAR_TYPE_SINT24, -1 };
static characteristic_type_t types_00002a69_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT32, CHAR_TYPE_UINT32, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a6a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_32BIT, -1 };
static characteristic_type_t types_00002a6b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, -1 };
static characteristic_type_t types_00002a6c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT24, -1 };
static characteristic_type_t types_00002a6d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT32, -1 };
static characteristic_type_t types_00002a6e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002a6f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a70_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a71_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a72_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a73_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a74_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a75_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT24, -1 };
static characteristic_type_t types_00002a76_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a77_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a78_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a79_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT8, -1 };
static characteristic_type_t types_00002a7a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT8, -1 };
static characteristic_type_t types_00002a7b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT8, -1 };
static characteristic_type_t types_00002a7d_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002a7e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a7f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a80_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a81_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a82_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a83_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a84_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a85_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a86_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a87_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a88_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a89_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a8a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a8b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a8c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a8d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a8e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a8f_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a90_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002a91_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a92_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a93_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a94_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a95_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a96_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a97_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a98_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a99_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT32, -1 };
static characteristic_type_t types_00002a9a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002a9b_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_32BIT, -1 };
static characteristic_type_t types_00002a9c_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a9d_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_8BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002a9e_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_32BIT, -1 };
static characteristic_type_t types_00002a9f_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002aa0_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002aa1_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002aa2_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002aa3_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002aa4_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, -1 };
static characteristic_type_t types_00002aa5_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_24BIT, -1 };
static characteristic_type_t types_00002aa6_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002aa7_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_8BIT, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT16, CHAR_TYPE_VARIABLE, CHAR_TYPE_SFLOAT, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002aa8_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_24BIT, CHAR_TYPE_4BIT, CHAR_TYPE_4BIT, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002aa9_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_24BIT, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002aaa_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002aab_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002aac_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_VARIABLE, CHAR_TYPE_UINT16, CHAR_TYPE_SFLOAT, CHAR_TYPE_UINT16, CHAR_TYPE_4BIT, CHAR_TYPE_4BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_8BIT, -1 };
static characteristic_type_t types_00002aad_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002aae_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT32, -1 };
static characteristic_type_t types_00002aaf_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT32, -1 };
static characteristic_type_t types_00002ab0_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002ab1_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002ab2_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002ab3_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ab4_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002ab5_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002ab6_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002ab7_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002ab8_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002ab9_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002aba_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002abb_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_BOOLEAN, -1 };
static characteristic_type_t types_00002abc_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002abd_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002abe_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UTF8S, -1 };
static characteristic_type_t types_00002abf_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_GATT_UUID, -1 };
static characteristic_type_t types_00002ac0_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT32, CHAR_TYPE_UINT32, -1 };
static characteristic_type_t types_00002ac1_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac2_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac3_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT48, -1 };
static characteristic_type_t types_00002ac4_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac5_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac6_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac7_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac8_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ac9_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002acc_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002acd_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT24, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, -1 };
static characteristic_type_t types_00002ace_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_24BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT24, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002acf_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad0_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad1_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT24, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad2_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_16BIT, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT24, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad3_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ad4_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad5_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad6_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad7_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002ad8_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_SINT16, CHAR_TYPE_SINT16, CHAR_TYPE_UINT16, -1 };
static characteristic_type_t types_00002ad9_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002ada_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002aed_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT24, -1 };
static characteristic_type_t types_00002b1d_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b1e_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b1f_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b20_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b21_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b22_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b23_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b24_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b25_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b26_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b27_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b28_0000_1000_8000_00805f9b34fb[] = { -1 };
static characteristic_type_t types_00002b29_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT8, -1 };
static characteristic_type_t types_00002b2a_0000_1000_8000_00805f9b34fb[] = { CHAR_TYPE_UINT128, -1 };

characteristic_desc_t characteristics[] = {
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00 },
        .name = "DeviceName",
        .types = types_00002a00_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x01, 0x2a, 0x00, 0x00 },
        .name = "Appearance",
        .types = types_00002a01_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x02, 0x2a, 0x00, 0x00 },
        .name = "PeripheralPrivacyFlag",
        .types = types_00002a02_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x03, 0x2a, 0x00, 0x00 },
        .name = "ReconnectionAddress",
        .types = types_00002a03_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x04, 0x2a, 0x00, 0x00 },
        .name = "PeripheralPreferredConnectionParameters",
        .types = types_00002a04_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x05, 0x2a, 0x00, 0x00 },
        .name = "ServiceChanged",
        .types = types_00002a05_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x06, 0x2a, 0x00, 0x00 },
        .name = "AlertLevel",
        .types = types_00002a06_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x07, 0x2a, 0x00, 0x00 },
        .name = "TxPowerLevel",
        .types = types_00002a07_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x08, 0x2a, 0x00, 0x00 },
        .name = "DateTime",
        .types = types_00002a08_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x09, 0x2a, 0x00, 0x00 },
        .name = "DayofWeek",
        .types = types_00002a09_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x2a, 0x00, 0x00 },
        .name = "DayDateTime",
        .types = types_00002a0a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0b, 0x2a, 0x00, 0x00 },
        .name = "ExactTime100",
        .types = types_00002a0b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0c, 0x2a, 0x00, 0x00 },
        .name = "ExactTime256",
        .types = types_00002a0c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0d, 0x2a, 0x00, 0x00 },
        .name = "DSTOffset",
        .types = types_00002a0d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0e, 0x2a, 0x00, 0x00 },
        .name = "TimeZone",
        .types = types_00002a0e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0f, 0x2a, 0x00, 0x00 },
        .name = "LocalTimeInformation",
        .types = types_00002a0f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x10, 0x2a, 0x00, 0x00 },
        .name = "SecondaryTimeZone",
        .types = types_00002a10_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0x2a, 0x00, 0x00 },
        .name = "TimewithDST",
        .types = types_00002a11_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x2a, 0x00, 0x00 },
        .name = "TimeAccuracy",
        .types = types_00002a12_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x13, 0x2a, 0x00, 0x00 },
        .name = "TimeSource",
        .types = types_00002a13_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x14, 0x2a, 0x00, 0x00 },
        .name = "ReferenceTimeInformation",
        .types = types_00002a14_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x15, 0x2a, 0x00, 0x00 },
        .name = "TimeBroadcast",
        .types = types_00002a15_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x16, 0x2a, 0x00, 0x00 },
        .name = "TimeUpdateControlPoint",
        .types = types_00002a16_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x17, 0x2a, 0x00, 0x00 },
        .name = "TimeUpdateState",
        .types = types_00002a17_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x18, 0x2a, 0x00, 0x00 },
        .name = "GlucoseMeasurement",
        .types = types_00002a18_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2a, 0x00, 0x00 },
        .name = "BatteryLevel",
        .types = types_00002a19_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1a, 0x2a, 0x00, 0x00 },
        .name = "BatteryPowerState",
        .types = types_00002a1a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1b, 0x2a, 0x00, 0x00 },
        .name = "BatteryLevelState",
        .types = types_00002a1b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1c, 0x2a, 0x00, 0x00 },
        .name = "TemperatureMeasurement",
        .types = types_00002a1c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1d, 0x2a, 0x00, 0x00 },
        .name = "TemperatureType",
        .types = types_00002a1d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1e, 0x2a, 0x00, 0x00 },
        .name = "IntermediateTemperature",
        .types = types_00002a1e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1f, 0x2a, 0x00, 0x00 },
        .name = "TemperatureCelsius",
        .types = types_00002a1f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x20, 0x2a, 0x00, 0x00 },
        .name = "TemperatureFahrenheit",
        .types = types_00002a20_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x21, 0x2a, 0x00, 0x00 },
        .name = "MeasurementInterval",
        .types = types_00002a21_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x22, 0x2a, 0x00, 0x00 },
        .name = "BootKeyboardInputReport",
        .types = types_00002a22_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x23, 0x2a, 0x00, 0x00 },
        .name = "SystemID",
        .types = types_00002a23_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x24, 0x2a, 0x00, 0x00 },
        .name = "ModelNumberString",
        .types = types_00002a24_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x25, 0x2a, 0x00, 0x00 },
        .name = "SerialNumberString",
        .types = types_00002a25_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x26, 0x2a, 0x00, 0x00 },
        .name = "FirmwareRevisionString",
        .types = types_00002a26_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x27, 0x2a, 0x00, 0x00 },
        .name = "HardwareRevisionString",
        .types = types_00002a27_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x28, 0x2a, 0x00, 0x00 },
        .name = "SoftwareRevisionString",
        .types = types_00002a28_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x29, 0x2a, 0x00, 0x00 },
        .name = "ManufacturerNameString",
        .types = types_00002a29_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x2a, 0x2a, 0x00, 0x00 },
        .name = "IEEE11073-20601RegulatoryCertificationDataList",
        .types = types_00002a2a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x2b, 0x2a, 0x00, 0x00 },
        .name = "CurrentTime",
        .types = types_00002a2b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x2c, 0x2a, 0x00, 0x00 },
        .name = "MagneticDeclination",
        .types = types_00002a2c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x2f, 0x2a, 0x00, 0x00 },
        .name = "Position2D",
        .types = types_00002a2f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x30, 0x2a, 0x00, 0x00 },
        .name = "Position3D",
        .types = types_00002a30_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x31, 0x2a, 0x00, 0x00 },
        .name = "ScanRefresh",
        .types = types_00002a31_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x32, 0x2a, 0x00, 0x00 },
        .name = "BootKeyboardOutputReport",
        .types = types_00002a32_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x33, 0x2a, 0x00, 0x00 },
        .name = "BootMouseInputReport",
        .types = types_00002a33_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x34, 0x2a, 0x00, 0x00 },
        .name = "GlucoseMeasurementContext",
        .types = types_00002a34_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x35, 0x2a, 0x00, 0x00 },
        .name = "BloodPressureMeasurement",
        .types = types_00002a35_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x36, 0x2a, 0x00, 0x00 },
        .name = "IntermediateCuffPressure",
        .types = types_00002a36_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x37, 0x2a, 0x00, 0x00 },
        .name = "HeartRateMeasurement",
        .types = types_00002a37_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x38, 0x2a, 0x00, 0x00 },
        .name = "BodySensorLocation",
        .types = types_00002a38_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x39, 0x2a, 0x00, 0x00 },
        .name = "HeartRateControlPoint",
        .types = types_00002a39_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3a, 0x2a, 0x00, 0x00 },
        .name = "Removable",
        .types = types_00002a3a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3b, 0x2a, 0x00, 0x00 },
        .name = "ServiceRequired",
        .types = types_00002a3b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3c, 0x2a, 0x00, 0x00 },
        .name = "ScientificTemperatureCelsius",
        .types = types_00002a3c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3d, 0x2a, 0x00, 0x00 },
        .name = "String",
        .types = types_00002a3d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3e, 0x2a, 0x00, 0x00 },
        .name = "NetworkAvailability",
        .types = types_00002a3e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x3f, 0x2a, 0x00, 0x00 },
        .name = "AlertStatus",
        .types = types_00002a3f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x40, 0x2a, 0x00, 0x00 },
        .name = "RingerControlpoint",
        .types = types_00002a40_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x41, 0x2a, 0x00, 0x00 },
        .name = "RingerSetting",
        .types = types_00002a41_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x42, 0x2a, 0x00, 0x00 },
        .name = "AlertCategoryIDBitMask",
        .types = types_00002a42_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x43, 0x2a, 0x00, 0x00 },
        .name = "AlertCategoryID",
        .types = types_00002a43_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x44, 0x2a, 0x00, 0x00 },
        .name = "AlertNotificationControlPoint",
        .types = types_00002a44_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x45, 0x2a, 0x00, 0x00 },
        .name = "UnreadAlertStatus",
        .types = types_00002a45_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x46, 0x2a, 0x00, 0x00 },
        .name = "NewAlert",
        .types = types_00002a46_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x47, 0x2a, 0x00, 0x00 },
        .name = "SupportedNewAlertCategory",
        .types = types_00002a47_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x48, 0x2a, 0x00, 0x00 },
        .name = "SupportedUnreadAlertCategory",
        .types = types_00002a48_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x49, 0x2a, 0x00, 0x00 },
        .name = "BloodPressureFeature",
        .types = types_00002a49_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x4a, 0x2a, 0x00, 0x00 },
        .name = "HIDInformation",
        .types = types_00002a4a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x4b, 0x2a, 0x00, 0x00 },
        .name = "ReportMap",
        .types = types_00002a4b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x4c, 0x2a, 0x00, 0x00 },
        .name = "HIDControlPoint",
        .types = types_00002a4c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x4d, 0x2a, 0x00, 0x00 },
        .name = "Report",
        .types = types_00002a4d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x4e, 0x2a, 0x00, 0x00 },
        .name = "ProtocolMode",
        .types = types_00002a4e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x4f, 0x2a, 0x00, 0x00 },
        .name = "ScanIntervalWindow",
        .types = types_00002a4f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x50, 0x2a, 0x00, 0x00 },
        .name = "PnPID",
        .types = types_00002a50_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x51, 0x2a, 0x00, 0x00 },
        .name = "GlucoseFeature",
        .types = types_00002a51_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x52, 0x2a, 0x00, 0x00 },
        .name = "RecordAccessControlPoint",
        .types = types_00002a52_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x53, 0x2a, 0x00, 0x00 },
        .name = "RSCMeasurement",
        .types = types_00002a53_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x54, 0x2a, 0x00, 0x00 },
        .name = "RSCFeature",
        .types = types_00002a54_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x55, 0x2a, 0x00, 0x00 },
        .name = "SCControlPoint",
        .types = types_00002a55_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x56, 0x2a, 0x00, 0x00 },
        .name = "Digital",
        .types = types_00002a56_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x57, 0x2a, 0x00, 0x00 },
        .name = "DigitalOutput",
        .types = types_00002a57_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x58, 0x2a, 0x00, 0x00 },
        .name = "Analog",
        .types = types_00002a58_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x59, 0x2a, 0x00, 0x00 },
        .name = "AnalogOutput",
        .types = types_00002a59_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x5a, 0x2a, 0x00, 0x00 },
        .name = "Aggregate",
        .types = types_00002a5a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x5b, 0x2a, 0x00, 0x00 },
        .name = "CSCMeasurement",
        .types = types_00002a5b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x5c, 0x2a, 0x00, 0x00 },
        .name = "CSCFeature",
        .types = types_00002a5c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x5d, 0x2a, 0x00, 0x00 },
        .name = "SensorLocation",
        .types = types_00002a5d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x5e, 0x2a, 0x00, 0x00 },
        .name = "PLXSpot-CheckMeasurement",
        .types = types_00002a5e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x5f, 0x2a, 0x00, 0x00 },
        .name = "PLXContinuousMeasurementCharacteristic",
        .types = types_00002a5f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x60, 0x2a, 0x00, 0x00 },
        .name = "PLXFeatures",
        .types = types_00002a60_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x62, 0x2a, 0x00, 0x00 },
        .name = "PulseOximetryControlPoint",
        .types = types_00002a62_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x63, 0x2a, 0x00, 0x00 },
        .name = "CyclingPowerMeasurement",
        .types = types_00002a63_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x64, 0x2a, 0x00, 0x00 },
        .name = "CyclingPowerVector",
        .types = types_00002a64_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x65, 0x2a, 0x00, 0x00 },
        .name = "CyclingPowerFeature",
        .types = types_00002a65_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x66, 0x2a, 0x00, 0x00 },
        .name = "CyclingPowerControlPoint",
        .types = types_00002a66_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x67, 0x2a, 0x00, 0x00 },
        .name = "LocationandSpeedCharacteristic",
        .types = types_00002a67_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x68, 0x2a, 0x00, 0x00 },
        .name = "Navigation",
        .types = types_00002a68_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x69, 0x2a, 0x00, 0x00 },
        .name = "PositionQuality",
        .types = types_00002a69_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x6a, 0x2a, 0x00, 0x00 },
        .name = "LNFeature",
        .types = types_00002a6a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x6b, 0x2a, 0x00, 0x00 },
        .name = "LNControlPoint",
        .types = types_00002a6b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x6c, 0x2a, 0x00, 0x00 },
        .name = "Elevation",
        .types = types_00002a6c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x6d, 0x2a, 0x00, 0x00 },
        .name = "Pressure",
        .types = types_00002a6d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x6e, 0x2a, 0x00, 0x00 },
        .name = "Temperature",
        .types = types_00002a6e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x6f, 0x2a, 0x00, 0x00 },
        .name = "Humidity",
        .types = types_00002a6f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x70, 0x2a, 0x00, 0x00 },
        .name = "TrueWindSpeed",
        .types = types_00002a70_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x71, 0x2a, 0x00, 0x00 },
        .name = "TrueWindDirection",
        .types = types_00002a71_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x72, 0x2a, 0x00, 0x00 },
        .name = "ApparentWindSpeed",
        .types = types_00002a72_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x73, 0x2a, 0x00, 0x00 },
        .name = "ApparentWindDirection",
        .types = types_00002a73_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x74, 0x2a, 0x00, 0x00 },
        .name = "GustFactor",
        .types = types_00002a74_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x75, 0x2a, 0x00, 0x00 },
        .name = "PollenConcentration",
        .types = types_00002a75_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x76, 0x2a, 0x00, 0x00 },
        .name = "UVIndex",
        .types = types_00002a76_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x77, 0x2a, 0x00, 0x00 },
        .name = "Irradiance",
        .types = types_00002a77_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x78, 0x2a, 0x00, 0x00 },
        .name = "Rainfall",
        .types = types_00002a78_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x79, 0x2a, 0x00, 0x00 },
        .name = "WindChill",
        .types = types_00002a79_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x7a, 0x2a, 0x00, 0x00 },
        .name = "HeatIndex",
        .types = types_00002a7a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x7b, 0x2a, 0x00, 0x00 },
        .name = "DewPoint",
        .types = types_00002a7b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x7d, 0x2a, 0x00, 0x00 },
        .name = "DescriptorValueChanged",
        .types = types_00002a7d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x7e, 0x2a, 0x00, 0x00 },
        .name = "AerobicHeartRateLowerLimit",
        .types = types_00002a7e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x7f, 0x2a, 0x00, 0x00 },
        .name = "AerobicThreshold",
        .types = types_00002a7f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x80, 0x2a, 0x00, 0x00 },
        .name = "Age",
        .types = types_00002a80_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x81, 0x2a, 0x00, 0x00 },
        .name = "AnaerobicHeartRateLowerLimit",
        .types = types_00002a81_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x82, 0x2a, 0x00, 0x00 },
        .name = "AnaerobicHeartRateUpperLimit",
        .types = types_00002a82_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x83, 0x2a, 0x00, 0x00 },
        .name = "AnaerobicThreshold",
        .types = types_00002a83_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x84, 0x2a, 0x00, 0x00 },
        .name = "AerobicHeartRateUpperLimit",
        .types = types_00002a84_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x85, 0x2a, 0x00, 0x00 },
        .name = "DateofBirth",
        .types = types_00002a85_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x86, 0x2a, 0x00, 0x00 },
        .name = "DateofThresholdAssessment",
        .types = types_00002a86_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x87, 0x2a, 0x00, 0x00 },
        .name = "EmailAddress",
        .types = types_00002a87_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x88, 0x2a, 0x00, 0x00 },
        .name = "FatBurnHeartRateLowerLimit",
        .types = types_00002a88_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x89, 0x2a, 0x00, 0x00 },
        .name = "FatBurnHeartRateUpperLimit",
        .types = types_00002a89_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x8a, 0x2a, 0x00, 0x00 },
        .name = "FirstName",
        .types = types_00002a8a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x8b, 0x2a, 0x00, 0x00 },
        .name = "FiveZoneHeartRateLimits",
        .types = types_00002a8b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x8c, 0x2a, 0x00, 0x00 },
        .name = "Gender",
        .types = types_00002a8c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x8d, 0x2a, 0x00, 0x00 },
        .name = "HeartRateMax",
        .types = types_00002a8d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x8e, 0x2a, 0x00, 0x00 },
        .name = "Height",
        .types = types_00002a8e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x8f, 0x2a, 0x00, 0x00 },
        .name = "HipCircumference",
        .types = types_00002a8f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x90, 0x2a, 0x00, 0x00 },
        .name = "LastName",
        .types = types_00002a90_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x91, 0x2a, 0x00, 0x00 },
        .name = "MaximumRecommendedHeartRate",
        .types = types_00002a91_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x92, 0x2a, 0x00, 0x00 },
        .name = "RestingHeartRate",
        .types = types_00002a92_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x93, 0x2a, 0x00, 0x00 },
        .name = "SportTypeforAerobicandAnaerobicThresholds",
        .types = types_00002a93_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x94, 0x2a, 0x00, 0x00 },
        .name = "ThreeZoneHeartRateLimits",
        .types = types_00002a94_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x95, 0x2a, 0x00, 0x00 },
        .name = "TwoZoneHeartRateLimit",
        .types = types_00002a95_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x96, 0x2a, 0x00, 0x00 },
        .name = "VO2Max",
        .types = types_00002a96_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x97, 0x2a, 0x00, 0x00 },
        .name = "WaistCircumference",
        .types = types_00002a97_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x98, 0x2a, 0x00, 0x00 },
        .name = "Weight",
        .types = types_00002a98_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x99, 0x2a, 0x00, 0x00 },
        .name = "DatabaseChangeIncrement",
        .types = types_00002a99_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x9a, 0x2a, 0x00, 0x00 },
        .name = "UserIndex",
        .types = types_00002a9a_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x9b, 0x2a, 0x00, 0x00 },
        .name = "BodyCompositionFeature",
        .types = types_00002a9b_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x9c, 0x2a, 0x00, 0x00 },
        .name = "BodyCompositionMeasurement",
        .types = types_00002a9c_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x9d, 0x2a, 0x00, 0x00 },
        .name = "WeightMeasurement",
        .types = types_00002a9d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x9e, 0x2a, 0x00, 0x00 },
        .name = "WeightScaleFeature",
        .types = types_00002a9e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x9f, 0x2a, 0x00, 0x00 },
        .name = "UserControlPoint",
        .types = types_00002a9f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa0, 0x2a, 0x00, 0x00 },
        .name = "MagneticFluxDensity-2D",
        .types = types_00002aa0_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa1, 0x2a, 0x00, 0x00 },
        .name = "MagneticFluxDensity-3D",
        .types = types_00002aa1_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa2, 0x2a, 0x00, 0x00 },
        .name = "Language",
        .types = types_00002aa2_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa3, 0x2a, 0x00, 0x00 },
        .name = "BarometricPressureTrend",
        .types = types_00002aa3_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa4, 0x2a, 0x00, 0x00 },
        .name = "BondManagementControlPoint",
        .types = types_00002aa4_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa5, 0x2a, 0x00, 0x00 },
        .name = "BondManagementFeatures",
        .types = types_00002aa5_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa6, 0x2a, 0x00, 0x00 },
        .name = "CentralAddressResolution",
        .types = types_00002aa6_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa7, 0x2a, 0x00, 0x00 },
        .name = "CGMMeasurement",
        .types = types_00002aa7_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa8, 0x2a, 0x00, 0x00 },
        .name = "CGMFeature",
        .types = types_00002aa8_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xa9, 0x2a, 0x00, 0x00 },
        .name = "CGMStatus",
        .types = types_00002aa9_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xaa, 0x2a, 0x00, 0x00 },
        .name = "CGMSessionStartTime",
        .types = types_00002aaa_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xab, 0x2a, 0x00, 0x00 },
        .name = "CGMSessionRunTime",
        .types = types_00002aab_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xac, 0x2a, 0x00, 0x00 },
        .name = "CGMSpecificOpsControlPoint",
        .types = types_00002aac_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xad, 0x2a, 0x00, 0x00 },
        .name = "IndoorPositioningConfiguration",
        .types = types_00002aad_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xae, 0x2a, 0x00, 0x00 },
        .name = "Latitude",
        .types = types_00002aae_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xaf, 0x2a, 0x00, 0x00 },
        .name = "Longitude",
        .types = types_00002aaf_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb0, 0x2a, 0x00, 0x00 },
        .name = "LocalNorthCoordinate",
        .types = types_00002ab0_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb1, 0x2a, 0x00, 0x00 },
        .name = "LocalEastCoordinate",
        .types = types_00002ab1_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb2, 0x2a, 0x00, 0x00 },
        .name = "FloorNumber",
        .types = types_00002ab2_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb3, 0x2a, 0x00, 0x00 },
        .name = "Altitude",
        .types = types_00002ab3_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb4, 0x2a, 0x00, 0x00 },
        .name = "Uncertainty",
        .types = types_00002ab4_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb5, 0x2a, 0x00, 0x00 },
        .name = "LocationName",
        .types = types_00002ab5_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb6, 0x2a, 0x00, 0x00 },
        .name = "URI",
        .types = types_00002ab6_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb7, 0x2a, 0x00, 0x00 },
        .name = "HTTPHeaders",
        .types = types_00002ab7_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb8, 0x2a, 0x00, 0x00 },
        .name = "HTTPStatusCode",
        .types = types_00002ab8_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xb9, 0x2a, 0x00, 0x00 },
        .name = "HTTPEntityBody",
        .types = types_00002ab9_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xba, 0x2a, 0x00, 0x00 },
        .name = "HTTPControlPoint",
        .types = types_00002aba_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xbb, 0x2a, 0x00, 0x00 },
        .name = "HTTPSSecurity",
        .types = types_00002abb_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xbc, 0x2a, 0x00, 0x00 },
        .name = "TDSControlPoint",
        .types = types_00002abc_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xbd, 0x2a, 0x00, 0x00 },
        .name = "OTSFeature",
        .types = types_00002abd_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xbe, 0x2a, 0x00, 0x00 },
        .name = "ObjectName",
        .types = types_00002abe_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xbf, 0x2a, 0x00, 0x00 },
        .name = "ObjectType",
        .types = types_00002abf_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc0, 0x2a, 0x00, 0x00 },
        .name = "ObjectSize",
        .types = types_00002ac0_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc1, 0x2a, 0x00, 0x00 },
        .name = "ObjectFirst-Created",
        .types = types_00002ac1_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc2, 0x2a, 0x00, 0x00 },
        .name = "ObjectLast-Modified",
        .types = types_00002ac2_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc3, 0x2a, 0x00, 0x00 },
        .name = "ObjectID",
        .types = types_00002ac3_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc4, 0x2a, 0x00, 0x00 },
        .name = "ObjectProperties",
        .types = types_00002ac4_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc5, 0x2a, 0x00, 0x00 },
        .name = "ObjectActionControlPoint",
        .types = types_00002ac5_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc6, 0x2a, 0x00, 0x00 },
        .name = "ObjectListControlPoint",
        .types = types_00002ac6_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc7, 0x2a, 0x00, 0x00 },
        .name = "ObjectListFilter",
        .types = types_00002ac7_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc8, 0x2a, 0x00, 0x00 },
        .name = "ObjectChanged",
        .types = types_00002ac8_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xc9, 0x2a, 0x00, 0x00 },
        .name = "ResolvablePrivateAddressOnly",
        .types = types_00002ac9_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xcc, 0x2a, 0x00, 0x00 },
        .name = "FitnessMachineFeature",
        .types = types_00002acc_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xcd, 0x2a, 0x00, 0x00 },
        .name = "TreadmillData",
        .types = types_00002acd_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xce, 0x2a, 0x00, 0x00 },
        .name = "CrossTrainerData",
        .types = types_00002ace_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xcf, 0x2a, 0x00, 0x00 },
        .name = "StepClimberData",
        .types = types_00002acf_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd0, 0x2a, 0x00, 0x00 },
        .name = "StairClimberData",
        .types = types_00002ad0_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd1, 0x2a, 0x00, 0x00 },
        .name = "RowerData",
        .types = types_00002ad1_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd2, 0x2a, 0x00, 0x00 },
        .name = "IndoorBikeData",
        .types = types_00002ad2_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd3, 0x2a, 0x00, 0x00 },
        .name = "TrainingStatus",
        .types = types_00002ad3_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd4, 0x2a, 0x00, 0x00 },
        .name = "SupportedSpeedRange",
        .types = types_00002ad4_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd5, 0x2a, 0x00, 0x00 },
        .name = "SupportedInclinationRange",
        .types = types_00002ad5_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd6, 0x2a, 0x00, 0x00 },
        .name = "SupportedResistanceLevelRange",
        .types = types_00002ad6_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd7, 0x2a, 0x00, 0x00 },
        .name = "SupportedHeartRateRange",
        .types = types_00002ad7_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd8, 0x2a, 0x00, 0x00 },
        .name = "SupportedPowerRange",
        .types = types_00002ad8_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xd9, 0x2a, 0x00, 0x00 },
        .name = "FitnessMachineControlPoint",
        .types = types_00002ad9_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xda, 0x2a, 0x00, 0x00 },
        .name = "FitnessMachineStatus",
        .types = types_00002ada_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xed, 0x2a, 0x00, 0x00 },
        .name = "DateUTC",
        .types = types_00002aed_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1d, 0x2b, 0x00, 0x00 },
        .name = "RCFeature",
        .types = types_00002b1d_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1e, 0x2b, 0x00, 0x00 },
        .name = "RCSettings",
        .types = types_00002b1e_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x1f, 0x2b, 0x00, 0x00 },
        .name = "ReconnectionConfigurationControlPoint",
        .types = types_00002b1f_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x20, 0x2b, 0x00, 0x00 },
        .name = "IDDStatusChanged",
        .types = types_00002b20_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x21, 0x2b, 0x00, 0x00 },
        .name = "IDDStatus",
        .types = types_00002b21_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x22, 0x2b, 0x00, 0x00 },
        .name = "IDDAnnunciationStatus",
        .types = types_00002b22_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x23, 0x2b, 0x00, 0x00 },
        .name = "IDDFeatures",
        .types = types_00002b23_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x24, 0x2b, 0x00, 0x00 },
        .name = "IDDStatusReaderControlPoint",
        .types = types_00002b24_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x25, 0x2b, 0x00, 0x00 },
        .name = "IDDCommandControlPoint",
        .types = types_00002b25_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x26, 0x2b, 0x00, 0x00 },
        .name = "IDDCommandData",
        .types = types_00002b26_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x27, 0x2b, 0x00, 0x00 },
        .name = "IDDRecordAccessControlPoint",
        .types = types_00002b27_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x28, 0x2b, 0x00, 0x00 },
        .name = "IDDHistoryData",
        .types = types_00002b28_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x29, 0x2b, 0x00, 0x00 },
        .name = "ClientSupportedFeatures",
        .types = types_00002b29_0000_1000_8000_00805f9b34fb,
    },
    {
        .uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x2a, 0x2b, 0x00, 0x00 },
        .name = "DatabaseHash",
        .types = types_00002b2a_0000_1000_8000_00805f9b34fb,
    },
    {}
};