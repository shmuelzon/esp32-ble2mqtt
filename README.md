# ESP32-BLE2MQTT

This project is a BLE to MQTT bridge, i.e. it exposes BLE GATT characteristics
as MQTT topics for bidirectional communication. It's developed for the ESP32 SoC
and is based on [ESP-IDF](https://github.com/espressif/esp-idf) release v3.2.2.

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
* `BLE2MQTT-XXX/Version` - The BLE2MQTT application version currently running
* `BLE2MQTT-XXX/ConfigVersion` - The BLE2MQTT configuration version currently
  loaded (MD5 hash of configuration file)
* `BLE2MQTT-XXX/Uptime` - The uptime of the ESP32, in seconds, published every
  minute

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
* For MiJia temperature and humidity sensors: `MACAddress`, `MessageCounter`,
  `Temperature`, `Humidity` and `BatteryLevel`
* For BeeWi Smart Door sensors: `Status` and `Battery`

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

## Remote Logging

If configured, the application can send the logs remotely via UDP to another
host to allow receiving logs from remote devices without a serial connection.
To receive these logs on your host, execute `make remote-monitor`.

## Configuration

The configuration file provided in located at
[data/config.json](data/config.json) in the repository. It contains all of the
different configuration options.

The `wifi` section below includes the following entries:
```json
{
  "wifi": {
    "ssid": "MY_SSID",
    "password": "MY_PASSWORD",
    "eap": {
      "method": null,
      "identity": null,
      "client_cert": null,
      "client_key": null,
      "server_cert": null,
      "username": null,
      "password": null
    }
  }
}
```
* `ssid` - The WiFi SSID the ESP32 should connect to
* `password` - The security password for the above network
* `eap` - WPA-Enterprise configuration (for enterprise networks only)
  * `method` - `TLS`, `PEAP` or `TTLS`
  * `identity` - The EAP identity
  * `ca_cert`, `client_cert`, `client_key` - Full path names, including a
    leading slash (/), of the certificate/key file (in PEM format) stored under
    the data folder
  * `username`, `password` - EAP login credentials

The `eth` section below includes the following entries:
```json
{
  "eth": {
    "clk_mode": "ETH_CLOCK_GPIO17_OUT",
    "phy": "LAN8720",
    "phy_gpio_power": 12
  }
}
```
* `clk_mode` - 
[Ethernet clock mode](https://docs.espressif.com/projects/esp-idf/en/release-v3.3/api-reference/network/esp_eth.html#_CPPv416eth_clock_mode_t),
one of:
  * `ETH_CLOCK_GPIO0_IN`
  * `ETH_CLOCK_GPIO0_OUT`
  * `ETH_CLOCK_GPIO16_OUT`
  * `ETH_CLOCK_GPIO17_OUT`
* `phy` - The PHY chip connected to ESP32 RMII, one of:
  * `LAN8720`
  * `TLK110`
  * `IP101`
* `phy_gpio_power` - GPIO pin controlling PHY power (optional)

_Note: Defining the `eth` section will disable WiFi_

The `mqtt` section below includes the following entries:
```json
{
  "mqtt": {
    "server": {
      "host": "192.168.1.1",
      "port": 1883,
      "ssl": false,
      "client_cert": null,
      "client_key": null,
      "server_cert": null,
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
  * `host` - Host name or IP address of the MQTT broker
  * `port` - TCP port of the MQTT broker. If not specificed will default to
    1883 or 8883, depending on SSL configuration
  * `client_cert`, `client_key`, `server_cert` - Full path names, including a
    leading slash (/), of the certificate/key file (in PEM format) stored under
    the data folder. For example, if a certificate file is placed at
    `data/certs/my_cert.pem`, the value stored in the configuration should be
    `/certs/my_cert.pem`
  * `username`, `password` - MQTT login credentials
  * `client_id` - The MQTT client ID
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
  entry will be connected. It's possible to use the wildcard character `?` to
  denote any value for a nibble.

    ```json
    "whitelist": [
      "aa:bb:cc:dd:ee:ff",
      "00:11:22:??:??:??"
    ]
    ```
* `services` - Add additional services or override a existing definitions to the
  ones grabbed automatically during build from http://www.bluetooth.org. Each
  service can include a `name` field which will be used in the MQTT topic
  instead of its UUID. In addition, it's possible to define a white/black list
  for discovered services. The white/black list UUIDs may contain the wildcard
  character `?` to denote any value for a nibble. For example:

    ```json
    "services": {
      "definitions": {
        "00002f00-0000-1000-8000-00805f9b34fb": {
          "name": "Relay Service"
        }
      },
      "blacklist": [
        "0000180a-0000-1000-8000-00805f9b34fb",
        "0000ffff-????-????-????-????????????"
      ]
    }
    ```
* `characteristics` - Add additional characteristics or override existing
  definitions to the ones grabbed automatically during build from
  http://www.bluetooth.org. Each characteristic can include a `name` field which
  will be used in the MQTT topic instead of its UUID and a `types` array
  defining how to parse the byte array reflecting the characteristic's value.
  In addition, it's possible to define a white/black list for discovered
  characteristics. The white/black list UUIDs may contain the wildcard character
  `?` to denote any value for a nibble. For example:

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
        "00002a29-0000-1000-8000-00805f9b34fb",
        "0000ffff-????-????-????-????????????"
      ]
    }
    ```
* `passkeys` - An object containing the passkey (number 000000~999999) that
  should be used for out-of-band authorization. Each entry is the MAC address of
  the BLE device and the value is the passkey to use. It's possible to use the
  wildcard character `?` to denote any value for a nibble.

    ```json
    "passkeys": {
      "aa:bb:cc:dd:ee:ff": 0,
      "00:11:22:??:??:??": 123456
    }
    ```

The optional `log` section below includes the following entries:
```json
{
  "log": {
    "host": "224.0.0.200",
    "port": 5000
  }
}
```
* `host` - The hostname or IP address to send the logs to. In case of an IP
  address, this may be a unicast, broadcast or multicast address
* `port` - The destination UDP port

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
