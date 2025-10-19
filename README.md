
```bash
idf.py set-target esp32-c3
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

ESP-NOW spezifisch:

CONFIG_ESPNOW_ENABLE=y - ESP-NOW aktiviert
CONFIG_ESPNOW_MAX_ENCRYPT_PEER_NUM=6 - Max. verschlüsselte Peers

Wichtige ESP32-C3 spezifische Einstellungen:

Ändere die server_mac Variable mit der MAC-Adresse deines Servers
Beide Geräte müssen auf Kanal 1 sein (oder ändere ESPNOW_WIFI_CHANNEL)

Single-Core (UNICORE) - C3 hat nur einen Core
RISC-V Architektur
Optimierte IRAM-Nutzung für WiFi
WPA3 Support aktiviert

Die Konfiguration ist optimiert für:

Niedrigen Stromverbrauch
Gute WiFi-Performance
ESP-NOW Kommunikation

### URL
* https://github.com/EzerLonginus/CH340E_USB_UART
* https://community.element14.com/technologies/open-source-hardware/b/blog/posts/ch340-building-yet-another-usb-c-to-uart-adapter-board