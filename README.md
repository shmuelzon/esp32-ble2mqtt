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

The configuration file provided in located at
[data/config.json](data/config.json) in the repository. It contains all of the
different configuration options.

The `wifi` section below includes the following entries:
```json
{
  "wifi": {
    "ssid": "MY_SSID",
    "password": "MY_PASSWORD"
  }
}
```
* `ssid` - The WiFi SSID the ESP32 should connect to
* `password` - The security password for the above network

The `mqtt` section below includes the following entries:
```json
{
  "mqtt": {
    "server": {
      "host": "192.168.1.1",
      "port": 1883,
      "username": null,
      "password": null,
      "client_it": null
    },
    "publish": {
      "qos": 0,
      "retain": true
    },
    "topics" :{
      "get_suffix": "/Get",
      "set_suffix": "/Set"
    }
  }
}
```
* `server` - defines the MQTT connection settings
* `publish` - configuration for publishing topics
* `topics`
  * `get_suffix` - Which suffix should be added to the MQTT value topic in order
    to issue a read request from the characteristic
  * `set_suffix` - Which suffix should be added to the MQTT value topic in order
    to write a new value to the characteristic

The `ble` section of the configuration file includes the following default
configuration:
```json
{
  "ble": {
    "//Optional: 'whitelist' or 'blacklist'": []
  }
}
```
* `whitelist`/`blacklist` - An array of MAC addresses of devices. If `whitelist`
  is used, only devices with a MAC address matching one of the entries will be
  connected while if `blacklist` is used, only devices that do not match any
  entry will be connected

    ```json
    "whitelist": [
      "aa:bb:cc:dd:ee:ff"
    ]
    ```
