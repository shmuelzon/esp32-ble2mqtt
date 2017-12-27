# ESP32-BLE2MQTT

This project aims to be a BLE to MQTT bridge, i.e. expose BLE GATT
characteristics as MQTT topics for bidirectional communication. It's developed
for the ESP32 SoC and is based on
[ESP-IDF](https://github.com/espressif/esp-idf) release 3.0.

For example, if a device with a MAC address of `a0:e6:f8:50:72:53` exposes the
[0000180f-0000-1000-8000-00805f9b34fb service](https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.battery_service.xml)
(Battery Service) which includes the
[00002a19-0000-1000-8000-00805f9b34fb characteristic](https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.battery_level.xml)
(Battery Level), the `a0:e6:f8:50:72:53/0000180f-0000-1000-8000-00805f9b34fb/00002a19-0000-1000-8000-00805f9b34fb`
MQTT topic is published with a value representing the battery level.

Characteristics supporting notifications will automatically be registered on and
new values will be published once available. It's also possible to proactively
issue a read request by publishing any value to the topic using the above format
suffixed with '/Get'. Note that values are raw data rather than converted to
strings and should be read as such.

In order to set a GATT value, publish a message to a writable characteristic
using the above format suffixed with `/Set`. Again, data is raw and not
converted to a string.

## Configuration

Configuration options for WiFi, MQTT and BLE are currently embedded in
[config.c](main/config.c).
They will be moved to a JSON file residing on an SPIFFS partition in the future.
