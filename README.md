# ESP32 SubGHz Device

A Flipper Zero-inspired SubGHz signal transmitter built on ESP32 hardware with CC1101 radio module. This is the initial release focused on transmitting pre-coded SubGHz signals.

## Table of Contents

- [Overview](#overview)
- [Current Features](#current-features)
- [Hardware Requirements](#hardware-requirements)
- [Architecture](#architecture)
  - [FreeRTOS Task Structure](#freertos-task-structure)
  - [Inter-Task Communication](#inter-task-communication)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Future Roadmap](#future-roadmap)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)

## Overview

This project is the **first version** of an ESP32-based SubGHz device designed to eventually become a Flipper Zero clone. Currently, it plays pre-coded Flipper Zero SubGHz files stored directly on the device. The firmware features a custom UI with animations, a multi-level menu system, and efficient FreeRTOS-based task management.

## Current Features

- ✅ **SubGHz Signal Transmission**: Transmit pre-coded signals using CC1101 radio module
- ✅ **Organized Signal Library**: Category-based signal organization
- ✅ **OLED Display Interface**: Custom UI with 128x64 OLED display
- ✅ **Button Navigation**: 4-button control scheme (Up, Down, Select, Back)
- ✅ **Animations**: Intro and menu animations


## Hardware Requirements

- **ESP32 Development Board** (any variant with dual cores)
- **CC1101 SubGHz Transceiver Module**
- **128x64 OLED Display** (I2C, compatible with U8g2 library)
- **4 Push Buttons** (Up, Down, Select, Back)
- **Power Supply** (USB or battery. Battery used was recycled from old disposable vape) 

### Pin Configuration

- **I2C (Display)**: SDA=21, SCL=22
- **Buttons**: Defined in `configs.h` (see `BUTTON_*_PIN` constants)
- **CC1101**: SPI pins (check CC1101 library configuration)

## Architecture

### FreeRTOS Task Structure

The firmware uses a **lock-free, queue-based architecture** with three independent tasks:

| Task | Core | Priority | Function |
|------|------|----------|----------|
| **ButtonTask** | 1 | 3 (Highest) | Scans buttons and sends events to queue |
| **DisplayTask** | 1 | 2 | Renders UI at 30 FPS based on menu state |
| **RadioTask** | 0 | 1 | Handles SubGHz transmission requests |

### Inter-Task Communication

All tasks communicate via **FreeRTOS queues** (no mutexes needed):

- `buttonQueue`: Button events → Main Loop
- `menuStateQueue`: Menu state → Display Task
- `transmitRequestQueue`: Transmit requests → Radio Task
- `transmitCompleteQueue`: Completion signals → Main Loop

This design ensures **thread-safe operation** without blocking or race conditions.

## Project Structure

```
├── main.cpp                 # Main firmware (this file)
├── button.h                 # Button debouncing and handling
├── display.h                # OLED display rendering
├── configs.h                # Pin definitions and constants
├── icon.h                   # Bitmap icon definitions
├── menu.h                   # Menu state management
├── radio.h                  # CC1101 radio control
├── animation.h              # Animation framework
└── generated_signals.h      # Pre-coded SubGHz signal database
```

## Usage

1. **Power on the device** - Intro animation plays automatically
2. **Navigate categories** - Use Up/Down buttons
3. **Select a category** - Press Select
4. **Choose a signal** - Use Up/Down, press Select
5. **View signal details** - Automatic on selection
6. **Transmit signal** - Press Select on details screen

## Future Roadmap

This is just the beginning! Planned features include:

- 📡 **Signal Recording**: Capture and save new SubGHz signals
- 💾 **SD Card Support**: Store unlimited signals on SD card
- 🔍 **Signal Analysis**: Decode and analyze captured signals
- 🛠️ **Signal Editor**: Modify and create custom signals
- 📶 **Additional Protocols**: RFID, NFC, Infrared support
- 🔋 **Battery Management**: Power monitoring and optimization
- 📱 **Wireless Config**: WiFi-based signal management

## Technical Details

- **Refresh Rate**: 30 FPS display updates
- **Button Debouncing**: Hardware debouncing with configurable scan rate
- **Radio Module**: ELECHOUSE CC1101 library
- **Display Library**: U8g2 (monochrome OLED)
- **Signal Format**: Flipper Zero `.sub` file format (converted to header arrays)

## Contributing

This is an early-stage project with lots of room for improvement! Contributions are welcome:

- Add new signal databases
- Improve UI/animations
- Optimize radio transmission
- Add new protocols
- Bug fixes and documentation

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
-- 
**⚠️ Legal Notice**: This device is intended for educational and authorized testing purposes only. Transmitting on regulated frequencies without proper authorization may be illegal in your jurisdiction. Users are responsible for complying with local laws and regulations.