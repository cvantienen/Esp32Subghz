#include "filesystem.h"
#include "signals.h"
#include <FS.h>
#include <LittleFS.h>

bool FilesystemHelper::begin(bool formatOnFail) {
    if (!LittleFS.begin(formatOnFail)) {
        Serial.println("[FS] LittleFS mount failed!");
        if (formatOnFail) {
            Serial.println("[FS] Formatting...");
            LittleFS.format();
            if (!LittleFS.begin()) {
                Serial.println("[FS] Format failed!");
                return false;
            }
        } else {
            return false;
        }
    }
    Serial.println("[FS] LittleFS mounted successfully");
    return true;
}

bool FilesystemHelper::isMounted() { return LittleFS.begin(); }

String FilesystemHelper::readFile(const char *path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("[FS] Failed to open file: %s\n", path);
        return String();
    }

    String content = file.readString();
    file.close();
    return content;
}

bool FilesystemHelper::readFile(const char *path, uint8_t *buffer,
                                size_t maxLen) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("[FS] Failed to open file: %s\n", path);
        return false;
    }

    size_t bytesRead = file.read(buffer, maxLen);
    file.close();
    return bytesRead > 0;
}

bool FilesystemHelper::readTextFile(const char *path, String &content) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("[FS] Failed to open file: %s\n", path);
        return false;
    }

    content = file.readString();
    file.close();
    return true;
}

bool FilesystemHelper::exists(const char *path) {
    return LittleFS.exists(path);
}

size_t FilesystemHelper::getFileSize(const char *path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        return 0;
    }
    size_t size = file.size();
    file.close();
    return size;
}

void FilesystemHelper::listDir(const char *dirname, uint8_t levels) {
    Serial.printf("[FS] Listing directory: %s\n", dirname);

    File root = LittleFS.open(dirname);
    if (!root) {
        Serial.println("[FS] Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("[FS] Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.printf("  DIR : %s\n", file.name());
            if (levels) {
                listDir(file.name(), levels - 1);
            }
        } else {
            Serial.printf("  FILE: %s  SIZE: %zu\n", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

// bool FilesystemHelper::parseFlipperFile(const char *path,
//                                         FlipperSignal &signal) {
//     String content = readFile(path);
//     if (content.length() == 0) {
//         return false;
//     }

//     // Extract frequency (in Hz, convert to MHz)
//     int freqPos = content.indexOf("Frequency: ");
//     if (freqPos == -1)
//         return false;
//     int freqEnd = content.indexOf("\n", freqPos);
//     String freqStr = content.substring(freqPos + 11, freqEnd);
//     signal.frequency = freqStr.toInt() / 1000000.0;

//     // Extract protocol
//     int protoPos = content.indexOf("Protocol: ");
//     if (protoPos != -1) {
//         int protoEnd = content.indexOf("\n", protoPos);
//         signal.protocol = content.substring(protoPos + 10, protoEnd);
//         signal.protocol.trim();
//     }

//     // Extract RAW_Data lines
//     signal.samples.clear();
//     int dataPos = 0;
//     while ((dataPos = content.indexOf("RAW_Data: ", dataPos)) != -1) {
//         int dataEnd = content.indexOf("\n", dataPos);
//         String dataLine = content.substring(dataPos + 10, dataEnd);

//         // Parse numbers from the line
//         int startIdx = 0;
//         while (startIdx < dataLine.length()) {
//             int spaceIdx = dataLine.indexOf(' ', startIdx);
//             if (spaceIdx == -1)
//                 spaceIdx = dataLine.length();

//             String numStr = dataLine.substring(startIdx, spaceIdx);
//             numStr.trim();
//             if (numStr.length() > 0) {
//                 signal.samples.push_back(numStr.toInt());
//             }

//             startIdx = spaceIdx + 1;
//         }

//         dataPos = dataEnd;
//     }

//     // Extract name from filename
//     String filename = String(path);
//     int lastSlash = filename.lastIndexOf('/');
//     int lastDot = filename.lastIndexOf('.');
//     if (lastSlash != -1) {
//         filename = filename.substring(lastSlash + 1);
//     }
//     if (lastDot != -1) {
//         filename = filename.substring(0, lastDot);
//     }
//     signal.name = filename;

//     return signal.samples.size() > 0;
// }

// bool FilesystemHelper::readBitmap(const char *path, uint8_t *buffer,
//                                   size_t maxSize, size_t &actualSize) {
//     File file = LittleFS.open(path, "r");
//     if (!file) {
//         Serial.printf("[FS] Failed to open bitmap: %s\n", path);
//         return false;
//     }

//     actualSize = file.size();
//     if (actualSize > maxSize) {
//         Serial.printf("[FS] Bitmap too large: %zu > %zu\n", actualSize,
//                       maxSize);
//         file.close();
//         return false;
//     }

//     size_t bytesRead = file.read(buffer, actualSize);
//     file.close();

//     return bytesRead == actualSize;
// }