#ifndef OTA_H
#define OTA_H

#include <Arduino.h>

// =============================================================================
// OTA CONFIGURATION
// =============================================================================

// WiFi credentials - CHANGE THESE!
#define WIFI_SSID "2.4 VAN"
#define WIFI_PASSWORD "Vantienen69"

// OTA Settings
#define OTA_HOSTNAME "esp32-subghz"
#define OTA_PASSWORD "your_ota_password"  // For ArduinoOTA security
#define WIFI_TIMEOUT_MS 15000             // 15 second connection timeout
#define OTA_IDLE_TIMEOUT_MS 300000        // 5 minute OTA mode timeout

// =============================================================================
// OTA STATE
// =============================================================================

enum class OTAState {
    IDLE,
    CONNECTING,
    READY,
    UPDATING,
    SUCCESS,
    FAILED,
    TIMEOUT
};

// =============================================================================
// OTA MANAGER CLASS
// =============================================================================

class OTAManager {
   public:
    OTAManager();

    // Core functions
    bool begin();           // Initialize WiFi + OTA
    void handle();          // Call in loop() when OTA mode active
    void end();             // Disconnect and cleanup

    // State management
    OTAState getState() const { return state_; }
    const char* getStateString() const;
    const char* getIPAddress() const;
    uint8_t getProgress() const { return progress_; }
    bool isActive() const { return state_ != OTAState::IDLE; }
    bool isUpdating() const { return state_ == OTAState::UPDATING; }

    // Callbacks (optional - set before begin())
    void setProgressCallback(void (*callback)(uint8_t progress));
    void setStartCallback(void (*callback)());
    void setEndCallback(void (*callback)(bool success));

   private:
    OTAState state_;
    uint8_t progress_;
    unsigned long connectStartTime_;
    unsigned long lastActivityTime_;
    char ipAddress_[16];

    void (*progressCallback_)(uint8_t);
    void (*startCallback_)();
    void (*endCallback_)(bool);

    void setupArduinoOTA();
    void setupElegantOTA();
};

// Global OTA manager instance
extern OTAManager otaManager;

#endif  // OTA_H