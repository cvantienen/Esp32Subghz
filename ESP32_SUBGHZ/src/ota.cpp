#include "ota.h"

#include <ArduinoOTA.h>
#include <WiFi.h>

// =============================================================================
// GLOBAL INSTANCES
// =============================================================================

OTAManager otaManager;

// =============================================================================
// CONSTRUCTOR
// =============================================================================

OTAManager::OTAManager()
    : state_(OTAState::IDLE),
      progress_(0),
      connectStartTime_(0) {
    ipAddress_[0] = '\0';
}

// =============================================================================
// STATE STRING
// =============================================================================

const char* OTAManager::getStateString() const {
    switch (state_) {
        case OTAState::IDLE:
            return "Idle";
        case OTAState::CONNECTING:
            return "Connecting...";
        case OTAState::READY:
            return "Ready";
        case OTAState::UPDATING:
            return "Updating...";
        case OTAState::SUCCESS:
            return "Success!";
        case OTAState::FAILED:
            return "Failed!";
        case OTAState::TIMEOUT:
            return "Timeout";
        default:
            return "Unknown";
    }
}

const char* OTAManager::getIPAddress() const { return ipAddress_; }

// =============================================================================
// BEGIN - Initialize WiFi and OTA (PlatformIO-style simple implementation)
// =============================================================================

bool OTAManager::begin() {
    if (state_ != OTAState::IDLE) {
        return false;  // Already running
    }

    Serial.println("[OTA] Starting OTA mode...");
    state_ = OTAState::CONNECTING;
    progress_ = 0;
    connectStartTime_ = millis();

    // Set WiFi mode and hostname
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(OTA_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.printf("[OTA] Connecting to %s\n", WIFI_SSID);

    // Wait for connection with timeout
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - connectStartTime_ > WIFI_TIMEOUT_MS) {
            Serial.println("[OTA] WiFi connection timeout!");
            state_ = OTAState::TIMEOUT;
            WiFi.disconnect(true);
            return false;
        }
        delay(100);
        Serial.print(".");
    }

    // Connected!
    Serial.println();
    Serial.printf("[OTA] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    strncpy(ipAddress_, WiFi.localIP().toString().c_str(), sizeof(ipAddress_) - 1);

    // Setup ArduinoOTA (PlatformIO-style simple setup)
    setupArduinoOTA();

    state_ = OTAState::READY;

    Serial.println("[OTA] OTA Ready!");
    Serial.printf("[OTA] ArduinoOTA hostname: %s.local\n", OTA_HOSTNAME);
    Serial.printf("[OTA] Use: pio run -t upload\n");

    return true;
}

// =============================================================================
// ARDUINO OTA SETUP - Simple PlatformIO-style implementation
// =============================================================================

void OTAManager::setupArduinoOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([this]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
        Serial.printf("[OTA] Start updating %s\n", type.c_str());
        state_ = OTAState::UPDATING;
        progress_ = 0;
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        progress_ = (progress * 100) / total;
        Serial.printf("[OTA] Progress: %u%%\r", progress_);
    });

    ArduinoOTA.onEnd([this]() {
        Serial.println("\n[OTA] Update complete!");
        state_ = OTAState::SUCCESS;
        progress_ = 100;
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]: ", error);
        switch (error) {
            case OTA_AUTH_ERROR:
                Serial.println("Auth Failed");
                break;
            case OTA_BEGIN_ERROR:
                Serial.println("Begin Failed");
                break;
            case OTA_CONNECT_ERROR:
                Serial.println("Connect Failed");
                break;
            case OTA_RECEIVE_ERROR:
                Serial.println("Receive Failed");
                break;
            case OTA_END_ERROR:
                Serial.println("End Failed");
                break;
        }
        state_ = OTAState::FAILED;
    });

    ArduinoOTA.begin();
}

// =============================================================================
// HANDLE - Call in loop() (PlatformIO-style simple handle)
// =============================================================================

void OTAManager::handle() {
    if (state_ == OTAState::IDLE) return;

    // Handle ArduinoOTA - simple PlatformIO-style
    ArduinoOTA.handle();
}

// =============================================================================
// END - Cleanup
// =============================================================================

void OTAManager::end() {
    Serial.println("[OTA] Ending OTA mode...");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    state_ = OTAState::IDLE;
    progress_ = 0;
    ipAddress_[0] = '\0';
}