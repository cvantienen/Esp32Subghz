#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "button.h"
#include "display.h"
#include "filesystem.h"
#include "icon.h"
#include "menu.h"
#include "radio.h"
#include "signals.h"

// =============================================================================
// CONSTANTS
// =============================================================================
#define BUTTON_SCAN_DELAY_MS 10
#define DISPLAY_REFRESH_MS 50
#define RADIO_CHECK_DELAY_MS 100
#define QUEUE_SIZE 10

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
Menu menu;

// =============================================================================
// FREERTOS QUEUES & MUTEXES
// =============================================================================
QueueHandle_t buttonQueue = NULL;
QueueHandle_t transmitRequestQueue = NULL;
QueueHandle_t transmitCompleteQueue = NULL;
SemaphoreHandle_t menuMutex = NULL;  // Protect menu access

// =============================================================================
// TASK 1: BUTTON SCANNER (Producer - sends button events)
// =============================================================================
void ButtonTask(void *parameter) {
    for (;;) {
        uint8_t buttonEvent = 0;
        
        if (button_up.pressed()) {
            buttonEvent = 1; // UP
            xQueueSend(buttonQueue, &buttonEvent, 0);
        }
        if (button_down.pressed()) {
            buttonEvent = 2; // DOWN
            xQueueSend(buttonQueue, &buttonEvent, 0);
        }
        if (button_select.pressed()) {
            buttonEvent = 3; // SELECT
            xQueueSend(buttonQueue, &buttonEvent, 0);
        }
        if (button_back.pressed()) {
            buttonEvent = 4; // BACK
            xQueueSend(buttonQueue, &buttonEvent, 0);
        }
        
        vTaskDelay(BUTTON_SCAN_DELAY_MS / portTICK_PERIOD_MS);
    }
}

// =============================================================================
// TASK 2: DISPLAY RENDERER (Consumer - reads menu state, draws screen)
// =============================================================================
void DisplayTask(void *parameter) {
    vTaskDelay(300 / portTICK_PERIOD_MS); // Wait longer for initialization
    
    for (;;) {
        // Lock menu while reading
        MenuScreen currentScreen;
        int selectedCategory, selectedSignal;
        int categoryPrev, categoryNext;
        int signalPrev, signalNext, signalCount;
        
        if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Capture all menu state atomically
            currentScreen = menu.getCurrentScreen();
            selectedCategory = menu.getSelectedCategory();
            selectedSignal = menu.getSelectedSignal();
            categoryPrev = menu.getCategoryPrev();
            categoryNext = menu.getCategoryNext();
            signalPrev = menu.getSignalPrev();
            signalNext = menu.getSignalNext();
            signalCount = menu.getSignalCount();
            xSemaphoreGive(menuMutex);
        } else {
            // Couldn't get lock, skip this frame
            vTaskDelay(DISPLAY_REFRESH_MS / portTICK_PERIOD_MS);
            continue;
        }
        
        // Now draw with captured values (no lock needed)
        display.clear();
        
        switch (currentScreen) {
        case MenuScreen::CATEGORIES:
            display.drawCategoryMenu(
                selectedCategory,
                categoryPrev,
                categoryNext,
                NUM_OF_CATEGORIES
            );
            break;
            
        case MenuScreen::SIGNALS:
            display.drawSignalMenu(
                SIGNAL_CATEGORIES[selectedCategory].name,
                SIGNAL_CATEGORIES[selectedCategory].signals,
                selectedSignal,
                signalPrev,
                signalNext,
                signalCount
            );
            break;
            
        case MenuScreen::DETAILS: {
            SubGHzSignal *signal =
                &SIGNAL_CATEGORIES[selectedCategory].signals[selectedSignal];
            display.drawSignalDetails(
                SIGNAL_CATEGORIES[selectedCategory].name,
                signal
            );
            break;
        }
        
        case MenuScreen::TRANSMIT: {
            SubGHzSignal *signal =
                &SIGNAL_CATEGORIES[selectedCategory].signals[selectedSignal];
            display.drawTransmitting(signal->name, signal->frequency);
            break;
        }
        
        default:
            break;
        }
        
        display.show();
        vTaskDelay(DISPLAY_REFRESH_MS / portTICK_PERIOD_MS);
    }
}

// =============================================================================
// TASK 3: RADIO HANDLER (Consumer - waits for transmit requests from queue)
// =============================================================================
void RadioTask(void *parameter) {
    for (;;) {
        uint8_t transmitRequest;
        
        // Wait for transmit request from queue (blocking)
        if (xQueueReceive(transmitRequestQueue, &transmitRequest, 
                         RADIO_CHECK_DELAY_MS / portTICK_PERIOD_MS) == pdTRUE) {
            
            // Check if still on TRANSMIT screen (with mutex)
            bool shouldTransmit = false;
            int category = 0;
            int sigIndex = 0;
            
            if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                if (menu.getCurrentScreen() == MenuScreen::TRANSMIT) {
                    shouldTransmit = true;
                    category = menu.getSelectedCategory();
                    sigIndex = menu.getSelectedSignal();
                }
                xSemaphoreGive(menuMutex);
            }
            
            if (shouldTransmit) {
                SubGHzSignal *signal =
                    &SIGNAL_CATEGORIES[category].signals[sigIndex];
                
                Serial.println("[RadioTask] Transmission started");
                radio.transmit(signal->samples, signal->length, signal->frequency);
                Serial.println("[RadioTask] Transmission complete");
                
                // Notify UI that transmission is complete
                uint8_t complete = 1;
                xQueueSend(transmitCompleteQueue, &complete, 0);
            }
        }
    }
}

// =============================================================================
// MAIN LOOP: UI CONTROLLER (Consumer - processes button events)
// =============================================================================
void loop() {
    uint8_t buttonEvent;
    
    // Process button events from queue
    if (xQueueReceive(buttonQueue, &buttonEvent, 20 / portTICK_PERIOD_MS)) {
        // Lock menu while modifying
        if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            switch (menu.getCurrentScreen()) {
            case MenuScreen::CATEGORIES:
                if (buttonEvent == 1) {
                    menu.categoryUp();
                } else if (buttonEvent == 2) {
                    menu.categoryDown();
                } else if (buttonEvent == 3) {
                    menu.setCurrentScreen(MenuScreen::SIGNALS);
                    menu.setSignalCount(
                        SIGNAL_CATEGORIES[menu.getSelectedCategory()].count
                    );
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
                    // User selected to transmit - send request to RadioTask
                    menu.setCurrentScreen(MenuScreen::TRANSMIT);
                    
                    // Send transmit request to RadioTask via queue
                    uint8_t request = 1;
                    xQueueSend(transmitRequestQueue, &request, 0);
                }
                break;
                
            case MenuScreen::TRANSMIT:
                if (buttonEvent == 4) {
                    // User cancelled - go back to details
                    menu.setCurrentScreen(MenuScreen::DETAILS);
                }
                break;
                
            default:
                break;
            }
            xSemaphoreGive(menuMutex);
        }
    }
    
    // Check for transmission complete
    uint8_t transmitComplete;
    if (xQueueReceive(transmitCompleteQueue, &transmitComplete, 0)) {
        // Transmission finished - return to DETAILS screen
        if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            menu.setCurrentScreen(MenuScreen::DETAILS);
            xSemaphoreGive(menuMutex);
        }
    }
}

// =============================================================================
// SETUP
// =============================================================================
void setup() {
    delay(200);
    Serial.begin(115200);
    delay(1200);
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
    delay(200);
    Serial.println("[setup] Display initialized");
    
    menu.setCurrentScreen(MenuScreen::CATEGORIES);
    menu.setCategoryCount(NUM_OF_CATEGORIES);
    
    FilesystemHelper::begin(true);
    
    // Create mutex for menu protection
    menuMutex = xSemaphoreCreateMutex();
    if (menuMutex == NULL) {
        Serial.println("[ERROR] Failed to create menu mutex!");
        while (1);
    }
    
    // Create queues
    buttonQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
    if (buttonQueue == NULL) {
        Serial.println("[ERROR] Failed to create button queue!");
        while (1);
    }
    
    transmitRequestQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
    if (transmitRequestQueue == NULL) {
        Serial.println("[ERROR] Failed to create transmit request queue!");
        while (1);
    }
    
    transmitCompleteQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
    if (transmitCompleteQueue == NULL) {
        Serial.println("[ERROR] Failed to create transmit complete queue!");
        while (1);
    }
    
    // Create tasks
    xTaskCreatePinnedToCore(
        ButtonTask,
        "ButtonTask",
        2500,
        NULL,
        3,
        NULL,
        1
    );
    
    xTaskCreatePinnedToCore(
        DisplayTask,
        "DisplayTask",
        5000,
        NULL,
        2,
        NULL,
        1
    );
    
    xTaskCreatePinnedToCore(
        RadioTask,
        "RadioTask",
        6000,
        NULL,
        1,
        NULL,
        0
    );
    
    Serial.println("[setup] Setup complete!");
}