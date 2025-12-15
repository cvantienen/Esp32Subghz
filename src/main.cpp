#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "button.h"
#include "display.h"
#include "filesystem.h"
#include "icon.h"
#include "menu.h"
#include "radio.h"
#include "animation.h"
#include "signals.h"

// =============================================================================
// CONSTANTS
// =============================================================================
#define BUTTON_SCAN_DELAY_MS 5
#define DISPLAY_REFRESH_MS 50 // 30 FPS (1000ms / 30 = 33.3ms)
#define RADIO_CHECK_DELAY_MS 100
#define QUEUE_SIZE 20

// =============================================================================
// MENU STATE STRUCTURE (sent via queue - no mutex needed!)
// =============================================================================
struct MenuState {
    MenuScreen screen;
    int8_t selectedCategory;
    int8_t selectedSignal;
    int8_t categoryPrev;
    int8_t categoryNext;
    int8_t signalPrev;
    int8_t signalNext;
    int8_t signalCount;
};

// =============================================================================
// TRANSMIT REQUEST STRUCTURE (includes menu state)
// =============================================================================
struct TransmitRequest {
    int8_t category;
    int8_t signalIndex;
};

// =============================================================================
// ANIMATION STATE STRUCTURE
// =============================================================================


// =============================================================================
// GLOBAL OBJECTS
// =============================================================================
const unsigned char *bitmap_icons[8] = {
    bitmap_icon_3dcube,     bitmap_icon_battery,   bitmap_icon_gauges,
    bitmap_icon_fireworks,  bitmap_icon_gps_speed, bitmap_icon_knob_over_oled,
    bitmap_icon_parksensor, bitmap_icon_turbo};

Button button_up(BUTTON_UP_PIN);
Button button_select(BUTTON_SELECT_PIN);
Button button_down(BUTTON_DOWN_PIN);
Button button_back(BUTTON_BACK_PIN);

SubghzRadio radio;
OledDisplay display(bitmap_icons);
Menu menu; // Only loop() modifies this - no mutex needed!

// =============================================================================
// FREERTOS QUEUES (all communication via queues - no mutex!)
// =============================================================================
QueueHandle_t buttonQueue = NULL;
QueueHandle_t menuStateQueue = NULL; // loop() → DisplayTask: menu state
QueueHandle_t transmitRequestQueue =
    NULL; // loop() → RadioTask: transmit request
QueueHandle_t transmitCompleteQueue = NULL; // RadioTask → loop(): completion

// =============================================================================
// TASK 1: BUTTON SCANNER (Producer - sends button events)
// =============================================================================
void ButtonTask(void *parameter) {
    for (;;) {
        if (button_up.pressed()) {
            uint8_t buttonEvent = 1;                  // UP
            xQueueSend(buttonQueue, &buttonEvent, 0); // Non-blocking
        }
        if (button_down.pressed()) {
            uint8_t buttonEvent = 2;                  // DOWN
            xQueueSend(buttonQueue, &buttonEvent, 0); // Non-blocking
        }
        if (button_select.pressed()) {
            uint8_t buttonEvent = 3;                  // SELECT
            xQueueSend(buttonQueue, &buttonEvent, 0); // Non-blocking
        }
        if (button_back.pressed()) {
            uint8_t buttonEvent = 4;                  // BACK
            xQueueSend(buttonQueue, &buttonEvent, 0); // Non-blocking
        }

        vTaskDelay(BUTTON_SCAN_DELAY_MS / portTICK_PERIOD_MS);
    }
}

// =============================================================================
// TASK 2: DISPLAY RENDERER (30 FPS - receives menu state from queue)
// =============================================================================
void DisplayTask(void *parameter) {
    vTaskDelay(300 / portTICK_PERIOD_MS); // Wait for initialization
    MenuState currentState;
    bool hasState = false;

    for (;;) {
        // Get latest menu state (non-blocking - use latest available)
        if (xQueueReceive(menuStateQueue, &currentState, 0) == pdTRUE) {
            hasState = true;
        }
        // Only draw if we have valid state
        if (hasState) {
            display.clear();

            switch (currentState.screen) {
            case MenuScreen::CATEGORIES:
                display.drawCategoryMenu(
                    currentState.selectedCategory, currentState.categoryPrev,
                    currentState.categoryNext, NUM_OF_CATEGORIES);
                break;

            case MenuScreen::SIGNALS:
                display.drawSignalMenu(
                    SIGNAL_CATEGORIES[currentState.selectedCategory].name,
                    SIGNAL_CATEGORIES[currentState.selectedCategory].signals,
                    currentState.selectedSignal, currentState.signalPrev,
                    currentState.signalNext, currentState.signalCount);
                break;

            case MenuScreen::DETAILS: {
                SubGHzSignal *signal =
                    &SIGNAL_CATEGORIES[currentState.selectedCategory]
                         .signals[currentState.selectedSignal];
                display.drawSignalDetails(
                    SIGNAL_CATEGORIES[currentState.selectedCategory].name,
                    signal);
                break;
            }

            case MenuScreen::TRANSMIT: {
                SubGHzSignal *signal =
                    &SIGNAL_CATEGORIES[currentState.selectedCategory]
                         .signals[currentState.selectedSignal];
                display.drawTransmitting(signal->name, signal->frequency);
                break;
            }
            
            case MenuScreen::INTRO: {
                display.drawMenuIntro();
                break;
            }
            case MenuScreen::STARTMENU{
                display.drawStartMenu(startMenuAnimation)
            }
            default:
                break;
            }

            display.show();
        }

        vTaskDelay(DISPLAY_REFRESH_MS / portTICK_PERIOD_MS);
    }
}

// =============================================================================
// TASK 3: RADIO HANDLER (receives transmit requests from queue)
// =============================================================================
void RadioTask(void *parameter) {
    for (;;) {
        TransmitRequest request;

        // Wait for transmit request (blocking)
        if (xQueueReceive(transmitRequestQueue, &request, portMAX_DELAY) ==
            pdTRUE) {

            SubGHzSignal *signal = &SIGNAL_CATEGORIES[request.category]
                                        .signals[request.signalIndex];

            Serial.println("[RadioTask] Transmission started");
            radio.transmit(signal->samples, signal->length, signal->frequency);
            Serial.println("[RadioTask] Transmission complete");

            // Notify UI that transmission is complete
            uint8_t complete = 1;
            xQueueSend(transmitCompleteQueue, &complete, 0);
        }
    }
}

// =============================================================================
// MAIN LOOP: UI CONTROLLER (owns menu, sends updates via queues)
// =============================================================================
void loop() {
    uint8_t buttonEvent;
    bool menuChanged = true; // Send initial state

    for (;;) {
        // Process all button events in queue
        while (xQueueReceive(buttonQueue, &buttonEvent, 0) == pdTRUE) {
            menuChanged = true; // Menu will change

            switch (menu.getCurrentScreen()) {
            case MenuScreen::CATEGORIES:
                if (buttonEvent == 1) {
                    menu.categoryUp();
                } else if (buttonEvent == 2) {
                    menu.categoryDown();
                } else if (buttonEvent == 3) {
                    menu.setCurrentScreen(MenuScreen::SIGNALS);
                    menu.setSignalCount(
                        SIGNAL_CATEGORIES[menu.getSelectedCategory()].count);
                    menu.resetSignal();
                }
                break;

            case MenuScreen::SIGNALS:
                if (buttonEvent == 1) {
                    menu.signalUp();
                } else if (buttonEvent == 2) {
                    menu.signalDown();
                } else if (buttonEvent == 4) {
                    menu.setCurrentScreen(MenuScreen::CATEGORIES);
                } else if (buttonEvent == 3) {
                    menu.setCurrentScreen(MenuScreen::DETAILS);
                }
                break;

            case MenuScreen::DETAILS:
                if (buttonEvent == 4) {
                    menu.setCurrentScreen(MenuScreen::SIGNALS);
                } else if (buttonEvent == 3) {
                    // User selected to transmit
                    menu.setCurrentScreen(MenuScreen::TRANSMIT);

                    // Send transmit request with menu state
                    TransmitRequest request;
                    request.category = menu.getSelectedCategory();
                    request.signalIndex = menu.getSelectedSignal();
                    xQueueSend(transmitRequestQueue, &request, 0);
                }
                break;

            case MenuScreen::TRANSMIT:
                if (buttonEvent == 4) {
                    menu.setCurrentScreen(MenuScreen::DETAILS);
                }
                break;
            case MenuScreen::INTRO:
                if (buttonEvent == 3) {
                    menu.setCurrentScreen(MenuScreen::CATEGORIES);
                }
                break;
            case MenuScreen:STARTMENU
            default:
                break;
            }
        }

        // Check for transmission complete
        uint8_t transmitComplete;
        if (xQueueReceive(transmitCompleteQueue, &transmitComplete, 0)) {
            menu.setCurrentScreen(MenuScreen::DETAILS);
            menuChanged = true;
        }

        // Send menu state update to DisplayTask (when changed)
        if (menuChanged) {
            MenuState state;
            state.screen = menu.getCurrentScreen();
            state.selectedCategory = menu.getSelectedCategory();
            state.selectedSignal = menu.getSelectedSignal();
            state.categoryPrev = menu.getCategoryPrev();
            state.categoryNext = menu.getCategoryNext();
            state.signalPrev = menu.getSignalPrev();
            state.signalNext = menu.getSignalNext();
            state.signalCount = menu.getSignalCount();

            // Send to DisplayTask (overwrite if queue full - always latest
            // state)
            xQueueOverwrite(menuStateQueue, &state);
            menuChanged = false;
        }

        // Small delay to prevent CPU hogging
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

// =============================================================================
// SETUP
// =============================================================================
void setup() {
    delay(200);
    Serial.begin(115200);
    delay(500);
    Serial.println("\n[setup] Booting ESP32...");

    // Initialize I2C FIRST (required for display)
    Serial.println("[setup] Initializing I2C...");
    Wire.begin(); // Default: SDA=21, SCL=22 on ESP32
    delay(200);
    Serial.println("[setup] I2C initialized");

    // Initialize hardware
    button_up.init();
    button_select.init();
    button_down.init();
    button_back.init();
    delay(50);

    radio.initCC1101();
    delay(50);

    // Initialize display
    Serial.println("[setup] Initializing display...");
    display.init();
    delay(50);
    display.clear();
    display.drawIntroScreen();
    display.show();
    delay(1500);
    Serial.println("[setup] Display initialized");

    menu.setCurrentScreen(MenuScreen::INTRO);
    menu.setCategoryCount(NUM_OF_CATEGORIES);

    FilesystemHelper::begin(true);

    // Create queues (NO MUTEX NEEDED!)
    buttonQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
    if (buttonQueue == NULL) {
        Serial.println("[ERROR] Failed to create button queue!");
        while (1)
            ;
    }

    // Menu state queue - size 1, always contains latest state
    menuStateQueue = xQueueCreate(1, sizeof(MenuState));
    if (menuStateQueue == NULL) {
        Serial.println("[ERROR] Failed to create menu state queue!");
        while (1)
            ;
    }

    transmitRequestQueue = xQueueCreate(QUEUE_SIZE, sizeof(TransmitRequest));
    if (transmitRequestQueue == NULL) {
        Serial.println("[ERROR] Failed to create transmit request queue!");
        while (1)
            ;
    }

    transmitCompleteQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
    if (transmitCompleteQueue == NULL) {
        Serial.println("[ERROR] Failed to create transmit complete queue!");
        while (1)
            ;
    }

    // Create tasks
    xTaskCreatePinnedToCore(ButtonTask, "ButtonTask", 2500, NULL, 3, NULL, 1);

    xTaskCreatePinnedToCore(DisplayTask, "DisplayTask", 5000, NULL, 2, NULL, 1);

    xTaskCreatePinnedToCore(RadioTask, "RadioTask", 6000, NULL, 1, NULL, 0);

    Serial.println("[setup] Setup complete!");
}