# ESP32-BLE2MQTT

This project is a BLE to MQTT bridge, i.e. it exposes BLE GATT characteristics
as MQTT topics for bidirectional communication. It's developed for the ESP32 SoC
and is based on [ESP-IDF](https://github.com/espressif/esp-idf) release v3.1.2.

For example, if a device with a MAC address of `a0:e6:f8:50:72:53` exposes the
[0000180f-0000-1000-8000-00805f9b34fb service](https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.battery_service.xml)
(Battery Service) which includes the
[00002a19-0000-1000-8000-00805f9b34fb characteristic](https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.battery_level.xml)
(Battery Level), the `a0:e6:f8:50:72:53/BatteryService/BatteryLevel` MQTT topic
is published with a value representing the battery level.

Characteristics supporting notifications will automatically be registered on and
new values will be published once available. It's also possible to proactively
issue a read request by publishing any value to the topic using the above format
suffixed with '/Get'. Note that values are strings representing the
characteristic values based on their definitions grabbed from
http://bluetooth.org. For example, a battery level of 100% (0x64) will be sent
as a string '100'.

In order to set a GATT value, publish a message to a writable characteristic
using the above format suffixed with `/Set`. Payload should be of the same
format described above and will be converted, when needed, before sending to the
BLE peripheral.

In addition to the characteristic values, the BLE2MQTT devices also publish
additional topics to help book-keeping:
* `<Peripheral MAC address>/Connected` - With a payload of `true`/`false`
  depicting if the peripheral is currently connected or not. Note that this
  topic is monitored by the BLE2MQTT instance currently connected to the
  peripheral so that if another instance publishes `false`, the current instance
  will re-publish `true`
* `<Peripheral MAC address>/Owner` - The name of the BLE2MQTT instance currently
  connected to the peripheral, e.g. `BLE2MQTT-XXXX`, where `XXXX` are the last 2
  octets of the ESP32's WiFi MAC address

## Broadcasters

Broadcasters are non-connectable BLE devices that only send advertisements.
This application supports publishing these advertisements over MQTT.
For each broadcaster, at-least two topics are published:
* `BLE2MQTT-XXXX/<Broadcaster MAC address>/Type` - The broadcaster type, e.g.
  `iBeacon`
* `BLE2MQTT-XXXX/<Broadcaster MAC address>/RSSI` - The RSSI value of the
  received advertisement

In addition, depending on the broadcaster type and payload, additional meta-data
is published.
* For iBeacon: `UUID`, `Major`, `Minor` and `Distance`
* For Eddystone:
  * `UID` frames: `Namespace`, `Instance` and `Distance`
  * `URL` frames: `URL` and `Distance`
  * `TLM` frames: `Voltage`, `Temperature`, `Count` and `Uptime`

**Note:** Broadcaster topics are published without the retained flag regardless
of what's defined in the configuration file.

## Compiling

Download the repository and its dependencies:
```bash
git clone --recursive https://github.com/shmuelzon/esp32-ble2mqtt
```
Modify the [configuration file](#configuration) to fit your environment, build
and flash (make sure to modify the serial device your ESP32 is connected to):
```bash
make flash
```

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
      "client_id": null
    },
    "publish": {
      "qos": 0,
      "retain": true
    },
    "topics" :{
      "prefix": "",
      "get_suffix": "/Get",
      "set_suffix": "/Set"
    }
  }
}
```
* `server` - MQTT connection parameters
* `publish` - Configuration for publishing topics
* `topics`
  * `prefix` - Which prefix should be added to all MQTT value topics. OTA
    related topics are already prefixed and are not affected by this value
  * `get_suffix` - Which suffix should be added to the MQTT value topic in order
    to issue a read request from the characteristic
  * `set_suffix` - Which suffix should be added to the MQTT value topic in order
    to write a new value to the characteristic

The `ble` section of the configuration file includes the following default
configuration:
```json
{
  "ble": {
    "//Optional: 'whitelist' or 'blacklist'": [],
    "services": {
      "definitions": {},
      "//Optional: 'whitelist' or 'blacklist'": []
    },
    "characteristics": {
      "definitions": {},
      "//Optional: 'whitelist' or 'blacklist'": []
    },
    "passkeys": {}
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
* `services` - Add additional services or override a existing definitions to the
  ones grabbed automatically during build from http://www.bluetooth.org. Each
  service can include a `name` field which will be used in the MQTT topic
  instead of its UUID. In addition, it's possible to define a white/black list
  for discovered services. For example:

    ```json
    "services": {
      "definitions": {
        "00002f00-0000-1000-8000-00805f9b34fb": {
          "name": "Relay Service"
        }
      },
      "blacklist": [
        "0000180a-0000-1000-8000-00805f9b34fb"
      ]
    }
    ```
* `characteristics` - Add additional characteristics or override existing
  definitions to the ones grabbed automatically during build from
  http://www.bluetooth.org. Each characteristic can include a `name` field which
  will be used in the MQTT topic instead of its UUID and a `types` array
  defining how to parse the byte array reflecting the characteristic's value.
  In addition, it's possible to define a white/black list for discovered
  characteristics. For example:

    ```json
    "characteristics": {
      "definitions": {
        "00002f01-0000-1000-8000-00805f9b34fb": {
          "name": "Relay State",
          "types": [
            "boolean"
          ]
        }
      },
      "blacklist": [
        "00002a29-0000-1000-8000-00805f9b34fb"
      ]
    }
    ```
* `passkeys` - An object containing the passkey (number 000000~999999) that
  should be used for out-of-band authorization. Each entry is the MAC address of
  the BLE device and the value is the passkey to use.

    ```json
    "passkeys": {
      "aa:bb:cc:dd:ee:ff": 000000
    }
    ```

## OTA

It is possible to upgrade both firmware and configuration file over-the-air once
an initial version was flashed via serial interface. To do so, execute:
`make upload` or `make upload-config` accordingly.
The above will upgrade all BLE2MQTT devices connected to the MQTT broker defined
in the configuration file. It is also possible to upgrade a specific device by
adding the `OTA_TARGET` variable to the above command set to the host name of
the requested device, e.g.:
```bash
make upload OTA_TARGET=BLE2MQTT-470C
```

Note: In order to avoid unneeded upgrades, there is a mechanism in place to
compare the new version with the one that resides on the flash. For the firmware
image it's based on the git tag and for the configuration file it's an MD5 hash
of its contents. In order to force an upgrade regardless of the currently
installed version, run `make force-upload` or `make force-upload-config`
respectively.
