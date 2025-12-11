# Flipper Zero SubGHz Signals

Place your Flipper Zero `.sub` files here to have them automatically converted to C++ code.

## Directory Structure

Organize signals by category using subdirectories:

```
flipper_signals/
├── TouchTunes/           # Category folder
│   ├── power.sub
│   ├── pause.sub
│   └── skip.sub
├── Garage/               # Another category
│   ├── door1.sub
│   └── door2.sub
├── Cars/
│   └── unlock.sub
└── random_signal.sub     # Root-level files go to "Misc" category
```

## How It Works

1. Add your `.sub` files to this directory (organize into folders for categories)
2. Build your project with PlatformIO
3. The pre-build script automatically generates:
   - `include/generated_signals.h`
   - `src/generated_signals.cpp`
4. Include the generated header in your code

## Manual Conversion

You can also run the converter manually:

```bash
python scripts/flipper_to_cpp.py
```

Or specify a custom input directory:

```bash
python scripts/flipper_to_cpp.py /path/to/your/sub/files
```

## Using Generated Signals in Code

```cpp
#include "generated_signals.h"

// Access all generated categories
for (int i = 0; i < NUM_GEN_CATEGORIES; i++) {
    SubghzSignalList& category = GEN_SIGNAL_CATEGORIES[i];
    Serial.println(category.name);
    
    for (int j = 0; j < category.count; j++) {
        SubGHzSignal& sig = category.signals[j];
        Serial.printf("  - %s (%.2f MHz)\n", sig.name, sig.frequency);
    }
}
```

## Supported .sub File Format

The converter supports standard Flipper Zero RAW SubGHz files:

```
Filetype: Flipper SubGhz RAW File
Version: 1
Frequency: 433920000
Preset: FuriHalSubGhzPresetOok650Async
Protocol: RAW
RAW_Data: 500 -1000 500 -500 1000 -200 ...
RAW_Data: 300 -600 ...
```

- **Frequency**: In Hz (will be converted to MHz)
- **RAW_Data**: Timing values in microseconds (positive = HIGH, negative = LOW)

