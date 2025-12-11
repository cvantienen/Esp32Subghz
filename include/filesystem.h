#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "signals.h"
#include <Arduino.h>
#include <LittleFS.h>

// Simple filesystem wrapper for ESP32 LittleFS
class FilesystemHelper {
  public:
    // Initialize LittleFS (call once in setup)
    static bool begin(bool formatOnFail = false);

    // Check if filesystem is mounted
    static bool isMounted();

    // Read entire file as String
    static String readFile(const char *path);

    // Read file into buffer
    static bool readFile(const char *path, uint8_t *buffer, size_t maxLen);

    // Read file into String (for text files)
    static bool readTextFile(const char *path, String &content);

    // Check if file exists
    static bool exists(const char *path);

    // Get file size
    static size_t getFileSize(const char *path);

    // List all files in a directory
    static void listDir(const char *dirname, uint8_t levels = 0);

    // Read Flipper .sub file and parse it
    // static bool parseFlipperFile(const char* path, FlipperSignal& signal);

    // // Read bitmap file (XBM format or raw binary)
    // static bool readBitmap(const char* path, uint8_t* buffer, size_t maxSize,
    // size_t& actualSize);
};

#endif