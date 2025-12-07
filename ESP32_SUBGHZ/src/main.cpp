#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <U8g2lib.h>

#include "button.h"
#include "display.h"
#include "icon.h"
#include "menu.h"
#include "ota.h"
#include "radio.h"
#include "signals.h"



// FPS COUNTER (add near the top, after includes)
// =============================================================================
unsigned long lastFpsTime = 0;
int frameCount = 0;
int currentFps = 0;

// =============================================================================
// ICONS (for display)
// =============================================================================
const unsigned char *bitmap_icons[8] = {
    bitmap_icon_3dcube,     bitmap_icon_battery,   bitmap_icon_gauges,
    bitmap_icon_fireworks,  bitmap_icon_gps_speed, bitmap_icon_knob_over_oled,
    bitmap_icon_parksensor, bitmap_icon_turbo};

// =============================================================================
// BUTTONS
// =============================================================================
Button button_up(BUTTON_UP_PIN);
Button button_select(BUTTON_SELECT_PIN);
Button button_down(BUTTON_DOWN_PIN);
Button button_back(BUTTON_BACK_PIN);

// =============================================================================
// RADIO OBJECT
// =============================================================================
SubghzRadio radio;

// =============================================================================
// DISPLAY OBJECT
// =============================================================================
OledDisplay display(bitmap_icons);

// =============================================================================
// MENU OBJECT
// =============================================================================
Menu menu;


// =============================================================================
// OTA MANAGER
// =============================================================================
void onOTAProgress(uint8_t progress) {
    display.clear();
    display.drawProgressBar("OTA Update", progress);
    display.show();
}

void onOTAStart() {
    Serial.println("[main] OTA update started");
}

void onOTAEnd(bool success) {
    display.clear();
    if (success) {
        display.drawCenteredText("Update Complete!", "Rebooting...");
    } else {
        display.drawCenteredText("Update Failed!", "Try again");
    }
    display.show();
    delay(2000);
    if (success) {
        ESP.restart();
    }
}

// =============================================================================
// SETUP
// =============================================================================
void setup() {
    delay(200);
    Serial.begin(115200);
    Serial.println("\n[setup] Booting ESP32...");

    // Initialize buttons
    button_up.init();
    button_select.init();
    button_down.init();
    button_back.init();

    // Check for OTA boot: Hold UP + DOWN during boot
    if (button_up.pressed() && button_down.pressed()) {
        Serial.println("[setup] OTA boot combo detected!");
        display.init();
        display.clear();
        display.drawCenteredText("OTA Mode", "Connecting...");
        display.show();

        // Setup OTA callbacks
        otaManager.setProgressCallback(onOTAProgress);
        otaManager.setStartCallback(onOTAStart);
        otaManager.setEndCallback(onOTAEnd);

        if (otaManager.begin()) {
            // Stay in OTA mode - main loop will handle it
            menu.setCurrentScreen(MenuScreen::OTA_MODE);
            return;
        }
    }
    radio.initCC1101();
    delay(50);
    display.init();
    delay(50);
    display.clear();
    display.drawIntroScreen();
    display.show();
    delay(1500);

    menu.setCurrentScreen(MenuScreen::CATEGORIES);
    menu.setCategoryCount(NUM_OF_CATEGORIES);
    Serial.println("[setup] Setup complete.");
}


void loop() {
    // FPS COUNTER
    frameCount++;
    if (millis() - lastFpsTime >= 1000) {
        currentFps = frameCount;
        frameCount = 0;
        lastFpsTime = millis();
        Serial.printf("FPS: %d\n", currentFps);  // Print to Serial Monitor

        
    }
    // =========================================================================
    // HANDLE INPUT (per-screen button logic)
    switch (menu.getCurrentScreen()) {
    case MenuScreen::CATEGORIES:
        if (button_up.pressed())
            menu.categoryUp();
        if (button_down.pressed())
            menu.categoryDown();
        if (button_select.pressed()) {
            menu.setCurrentScreen(MenuScreen::SIGNALS);
            menu.setSignalCount(
                SIGNAL_CATEGORIES[menu.getSelectedCategory()].count);
            menu.resetSignal();
        }
        break;

    case MenuScreen::SIGNALS:
        if (button_up.pressed())
            menu.signalUp();
        if (button_down.pressed())
            menu.signalDown();
        if (button_back.pressed())
            menu.setCurrentScreen(MenuScreen::CATEGORIES);
        if (button_select.pressed())
            menu.setCurrentScreen(MenuScreen::DETAILS);
        break;

    case MenuScreen::DETAILS:
        if (button_back.pressed())
            menu.setCurrentScreen(MenuScreen::SIGNALS);
        if (button_select.pressed())
            menu.setCurrentScreen(MenuScreen::TRANSMIT);
        break;

    case MenuScreen::TRANSMIT:
        break;

    case MenuScreen::OTA_MODE:
        otaManager.handle();

        // Exit OTA on timeout or back button (if not updating)
        if (!otaManager.isUpdating()) {
            if (button_back.pressed() ||
                otaManager.getState() == OTAState::TIMEOUT ||
                otaManager.getState() == OTAState::FAILED) {
                otaManager.end();
                menu.setCurrentScreen(MenuScreen::CATEGORIES);
                // Re-init radio after WiFi
                radio.initCC1101();
            }
        }

        // Draw OTA screen
        display.clear();
        if (otaManager.isUpdating()) {
            display.drawProgressBar("Updating...", otaManager.getProgress());
        } else {
            display.drawOTAScreen(otaManager.getIPAddress(), otaManager.getStateString());
        }
        display.show();
        break;
    }

    // =========================================================================
    // DRAW CURRENT SCREEN
    // Clear buffer, draw based on current_screen, then show.
    display.clear();

    if (menu.getCurrentScreen() == MenuScreen::CATEGORIES) {
        // Draw category list with prev/current/next visible
        display.drawCategoryMenu(menu.getSelectedCategory(),
                                 menu.getCategoryPrev(), menu.getCategoryNext(),
                                 NUM_OF_CATEGORIES);
    } else if (menu.getCurrentScreen() == MenuScreen::SIGNALS) {
        // Draw signal list for selected category
        display.drawSignalMenu(
            SIGNAL_CATEGORIES[menu.getSelectedCategory()].name,
            SIGNAL_CATEGORIES[menu.getSelectedCategory()].signals,
            menu.getSelectedSignal(), menu.getSignalPrev(),
            menu.getSignalNext(), menu.getSignalCount());
    } else if (menu.getCurrentScreen() == MenuScreen::DETAILS) {
        // Get the selected signal
        SubGHzSignal *signal = &SIGNAL_CATEGORIES[menu.getSelectedCategory()]
                                    .signals[menu.getSelectedSignal()];
        // Draw signal details
        display.drawSignalDetails(
            SIGNAL_CATEGORIES[menu.getSelectedCategory()].name, signal);
    } else if (menu.getCurrentScreen() == MenuScreen::TRANSMIT) {
        // Get the selected signal
        SubGHzSignal *signal = &SIGNAL_CATEGORIES[menu.getSelectedCategory()]
                                    .signals[menu.getSelectedSignal()];
        // Draw transmitting screen
        display.drawTransmitting(signal->name, signal->frequency);
        // Transmit the signal
        radio.transmit(signal->samples, signal->length, signal->frequency);
        // Return to the DETAILS screen
        menu.setCurrentScreen(MenuScreen::DETAILS);
    }

    display.show();
}