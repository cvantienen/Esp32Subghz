# 📘 ESP32 PlatformIO – Build, Flash & OTA Commands


## 📝 Overview

This README provides quick-access commands for working with ESP32 projects using PlatformIO.
All commands use the full PlatformIO binary path:

```
~/.platformio/penv/bin/pio
```

Run all commands inside your PlatformIO project directory.

---

## 🔧 Build Commands

```
~/.platformio/penv/bin/pio run
~/.platformio/penv/bin/pio run -t clean
~/.platformio/penv/bin/pio run -t upload
~/.platformio/penv/bin/pio run -t upload --upload-port /dev/ttyUSB0

~/.platformio/penv/bin/pio device monitor
~/.platformio/penv/bin/pio device monitor -b 115200
```

---

## 📂 Filesystem Commands (LittleFS / SPIFFS)

```
~/.platformio/penv/bin/pio run -t buildfs
~/.platformio/penv/bin/pio run -t uploadfs
~/.platformio/penv/bin/pio run -t uploadfs --upload-port /dev/ttyUSB0
```

---

## 💾 Flash & Memory Tools

```
~/.platformio/penv/bin/pio run -t erase
~/.platformio/penv/bin/pio run -t size
~/.platformio/penv/bin/pio run -t sizepre
```

---

## 📡 OTA Update Commands

### Upload firmware via OTA (network upload)

```
~/.platformio/penv/bin/pio run -t upload --upload-port <DEVICE_IP>
```

Example:

```
~/.platformio/penv/bin/pio run -t upload --upload-port 192.168.1.42
```

### For ESP32 ArduinoOTA, the port is usually `3232`

```
~/.platformio/penv/bin/pio run -t upload --upload-port 192.168.1.42:3232
```

### Discover OTA-enabled devices on the network

```
~/.platformio/penv/bin/pio device list --mdns
```

### If using ElegantOTA Web Interface

(Upload through your browser instead of CLI)

```
Open http://<DEVICE_IP>/update in your browser
```

---

## 🚀 Workflow Examples

### Full Clean → Build → Upload (USB)

```
~/.platformio/penv/bin/pio run -t clean && ~/.platformio/penv/bin/pio run && ~/.platformio/penv/bin/pio run -t upload
```

### Build → Upload → Open Serial Monitor

```
~/.platformio/penv/bin/pio run && ~/.platformio/penv/bin/pio run -t upload && ~/.platformio/penv/bin/pio device monitor
```

### OTA Workflow (build → OTA upload)

```
~/.platformio/penv/bin/pio run && ~/.platformio/penv/bin/pio run -t upload --upload-port 192.168.1.42
```

---

