#include "ota.h"

#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <WiFi.h>

// =============================================================================
// GLOBAL INSTANCES
// =============================================================================

OTAManager otaManager;
AsyncWebServer otaServer(80);

// =============================================================================
// CONSTRUCTOR
// =============================================================================

OTAManager::OTAManager()
    : state_(OTAState::IDLE),
      progress_(0),
      connectStartTime_(0),
      lastActivityTime_(0),
      progressCallback_(nullptr),
      startCallback_(nullptr),
      endCallback_(nullptr) {
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
// CALLBACKS
// =============================================================================

void OTAManager::setProgressCallback(void (*callback)(uint8_t progress)) {
    progressCallback_ = callback;
}

void OTAManager::setStartCallback(void (*callback)()) {
    startCallback_ = callback;
}

void OTAManager::setEndCallback(void (*callback)(bool success)) {
    endCallback_ = callback;
}

// =============================================================================
// BEGIN - Initialize WiFi and OTA
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

    // Setup OTA services
    setupArduinoOTA();
    setupElegantOTA();

    state_ = OTAState::READY;
    lastActivityTime_ = millis();

    Serial.println("[OTA] OTA Ready!");
    Serial.printf("[OTA] Web UI: http://%s/update\n", ipAddress_);
    Serial.printf("[OTA] ArduinoOTA hostname: %s.local\n", OTA_HOSTNAME);

    return true;
}

// =============================================================================
// ARDUINO OTA SETUP (for IDE uploads)
// =============================================================================

void OTAManager::setupArduinoOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([this]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
        Serial.printf("[OTA] Start updating %s\n", type.c_str());
        state_ = OTAState::UPDATING;
        progress_ = 0;
        if (startCallback_) startCallback_();
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        progress_ = (progress * 100) / total;
        Serial.printf("[OTA] Progress: %u%%\r", progress_);
        lastActivityTime_ = millis();
        if (progressCallback_) progressCallback_(progress_);
    });

    ArduinoOTA.onEnd([this]() {
        Serial.println("\n[OTA] Update complete!");
        state_ = OTAState::SUCCESS;
        progress_ = 100;
        if (endCallback_) endCallback_(true);
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
        if (endCallback_) endCallback_(false);
    });

    ArduinoOTA.begin();
}

// =============================================================================
// ELEGANT OTA SETUP (Web UI)
// =============================================================================

void OTAManager::setupElegantOTA() {
    // Simple status page at root
    otaServer.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        String html = "<!DOCTYPE html><html><head>";
        html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
        html += "<style>body{font-family:monospace;background:#1a1a2e;color:#eee;";
        html += "display:flex;flex-direction:column;align-items:center;padding:40px;}";
        html += "h1{color:#00d4ff;}a{color:#00ff88;font-size:1.2em;}</style></head>";
        html += "<body><h1>ESP32 SubGHz</h1>";
        html += "<p>Status: " + String(getStateString()) + "</p>";
        html += "<p>IP: " + String(ipAddress_) + "</p>";
        html += "<p><a href='/update'>Open OTA Update Page</a></p>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    // Setup ElegantOTA with callbacks
    ElegantOTA.begin(&otaServer);

    ElegantOTA.onStart([this]() {
        Serial.println("[OTA] Web update started");
        state_ = OTAState::UPDATING;
        progress_ = 0;
        if (startCallback_) startCallback_();
    });

    ElegantOTA.onProgress([this](size_t current, size_t total) {
        progress_ = (current * 100) / total;
        lastActivityTime_ = millis();
        if (progressCallback_) progressCallback_(progress_);
    });

    ElegantOTA.onEnd([this](bool success) {
        Serial.printf("[OTA] Web update %s\n", success ? "success" : "failed");
        state_ = success ? OTAState::SUCCESS : OTAState::FAILED;
        if (endCallback_) endCallback_(success);
    });

    otaServer.begin();
}

// =============================================================================
// HANDLE - Call in loop()
// =============================================================================

void OTAManager::handle() {
    if (state_ == OTAState::IDLE) return;

    // Handle ArduinoOTA
    ArduinoOTA.handle();

    // Handle ElegantOTA (async, handled automatically)
    ElegantOTA.loop();

    // Check for idle timeout (only when READY, not during update)
    if (state_ == OTAState::READY) {
        if (millis() - lastActivityTime_ > OTA_IDLE_TIMEOUT_MS) {
            Serial.println("[OTA] Idle timeout, exiting OTA mode");
            state_ = OTAState::TIMEOUT;
        }
    }
}

// =============================================================================
// END - Cleanup
// =============================================================================

void OTAManager::end() {
    Serial.println("[OTA] Ending OTA mode...");
    otaServer.end();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    state_ = OTAState::IDLE;
    progress_ = 0;
    ipAddress_[0] = '\0';
}