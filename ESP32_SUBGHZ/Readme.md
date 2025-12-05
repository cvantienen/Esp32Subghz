PlatformIO ESP32 Command-Line Cheat Sheet

Basic Commands

Build

pio run

Compile the project and generate firmware binaries.

Clean

pio run -t clean

Remove all compiled objects and cached build files.

Upload Firmware

pio run -t upload

Flash compiled firmware to the ESP32.

Upload Firmware to Specific Port

pio run -t upload --upload-port /dev/ttyUSB0

Specify the serial port manually.

Serial Monitor

pio device monitor

Open the serial monitor.

pio device monitor -b 115200

Open monitor using a specific baud rate.

Filesystem Commands (LittleFS/SPIFFS)

Build Filesystem Image

pio run -t buildfs

Generate the filesystem image from the data/ directory.

Upload Filesystem Image

pio run -t uploadfs

Flash the filesystem image to the ESP32.

Upload Filesystem to Specific Port

pio run -t uploadfs --upload-port /dev/ttyUSB0

Manually select upload port.

Flash and Memory Commands

Erase Flash Chip

pio run -t erase

Erase all flash memory, including firmware, filesystem, and NVS.

Firmware Size

pio run -t size

Display compiled program flash and RAM usage.

Pre-Linking Size

pio run -t sizepre

Show size information before final linking.

Advanced Commands

Program Using External Programmer

pio run -t program

Upload firmware using JTAG or an external programmer instead of USB.

ESP-IDF Menuconfig (ESP-IDF Framework Only)

pio run -t menuconfig

Open the ESP-IDF configuration menu.

Typical Workflow

Full Clean, Build, and Upload

pio run -t clean && pio run && pio run -t upload

Build, Upload, and Open Serial Monitor

pio run && pio run -t upload && pio device monitor

Notes

All commands must be run inside a PlatformIO project directory.

The platformio.ini file controls board type, upload settings, filesystem type, and more.
