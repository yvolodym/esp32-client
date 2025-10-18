```bash
git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git
cd ESP8266_RTOS_SDK
./install.sh
. ./export.sh
```

```bash
idf.py set-target esp32-c3
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

ESP-NOW spezifisch:

CONFIG_ESPNOW_ENABLE=y - ESP-NOW aktiviert
CONFIG_ESPNOW_MAX_ENCRYPT_PEER_NUM=6 - Max. verschlüsselte Peers

WiFi Konfiguration:

Station Mode aktiv
2MB Flash
115200 Baud für Monitor
NVS aktiviert

Wichtige Parameter:

Main Task Stack: 3584 Bytes
WiFi PPT Task Stack: 5120 Bytes
TCP Connections: 5 aktive, 5 listening
Log Level: INFO