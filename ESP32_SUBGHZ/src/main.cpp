#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <U8g2lib.h>

#include "button.h"
#include "icon.h"
#include "radio.h"
#include "signals.h"
#include "display.h"
#include "menu.h"


// =============================================================================
// ICONS (for display)
// =============================================================================
const unsigned char *bitmap_icons[8] = {
    bitmap_icon_3dcube,
    bitmap_icon_battery,
    bitmap_icon_gauges,
    bitmap_icon_fireworks,
    bitmap_icon_gps_speed,
    bitmap_icon_knob_over_oled,
    bitmap_icon_parksensor,
    bitmap_icon_turbo
};

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
// SETUP
// =============================================================================
void setup()
{
    Serial.begin(115200);
    Serial.println("\n[setup] Booting ESP32...");

    button_up.init();
    button_select.init();
    button_down.init();
    button_back.init();

    radio.initCC1101();
    display.init();

    menu.setCurrentScreen(MenuScreen::CATEGORIES);
    menu.setCategoryCount(NUM_OF_CATEGORIES);
}

// =============================================================================
// MAIN LOOP
// =============================================================================
// The loop does 3 things every frame:
//   1. HANDLE INPUT  - Check buttons, update state
//   2. CALCULATE     - Compute prev/next indices for display
//   3. DRAW          - Render the current screen
//
// REFACTOR: Split into menu.handleInput(), menu.update(), display.draw(menu)

void loop()
{
    // =========================================================================
    // HANDLE INPUT (per-screen button logic)
    switch (menu.getCurrentScreen())
    {
        case MenuScreen::CATEGORIES:
            if (button_up.pressed())   menu.categoryUp();
            if (button_down.pressed()) menu.categoryDown();
            if (button_select.pressed()) {
                menu.setCurrentScreen(MenuScreen::SIGNALS);
                menu.setSignalCount(SIGNAL_CATEGORIES[menu.getSelectedCategory()].count);
                menu.resetSignal();
            }
            break;

        case MenuScreen::SIGNALS:
            if (button_up.pressed())   menu.signalUp();
            if (button_down.pressed()) menu.signalDown();
            if (button_back.pressed()) menu.setCurrentScreen(MenuScreen::CATEGORIES);
            if (button_select.pressed()) menu.setCurrentScreen(MenuScreen::DETAILS);
            break;

        case MenuScreen::DETAILS:
            if (button_back.pressed())   menu.setCurrentScreen(MenuScreen::SIGNALS);
            if (button_select.pressed()) menu.setCurrentScreen(MenuScreen::TRANSMIT);
            break;

        case MenuScreen::TRANSMIT:
            // Handled in draw section
            break;
    }

    // =========================================================================
    // DRAW CURRENT SCREEN
    // Clear buffer, draw based on current_screen, then show.
    display.clear();

    if (menu.getCurrentScreen() == MenuScreen::CATEGORIES)
    {
        // Draw category list with prev/current/next visible
        display.drawCategoryMenu(
            menu.getSelectedCategory(),
            menu.getCategoryPrev(),
            menu.getCategoryNext(),
            NUM_OF_CATEGORIES
        );
    }
    else if (menu.getCurrentScreen() == MenuScreen::SIGNALS)
    {   
        // Draw signal list for selected category
        display.drawSignalMenu(
            SIGNAL_CATEGORIES[menu.getSelectedCategory()].name,
            SIGNAL_CATEGORIES[menu.getSelectedCategory()].signals,
            menu.getSelectedSignal(),
            menu.getSignalPrev(),
            menu.getSignalNext(),
            menu.getSignalCount()
        );
    }
    else if (menu.getCurrentScreen() == MenuScreen::DETAILS)
    {
        // Get the selected signal
        SubGHzSignal *signal = &SIGNAL_CATEGORIES[menu.getSelectedCategory()].signals[menu.getSelectedSignal()];
        // Draw signal details
        display.drawSignalDetails(SIGNAL_CATEGORIES[menu.getSelectedCategory()].name, signal);
    }
    else if (menu.getCurrentScreen() == MenuScreen::TRANSMIT)
    {
        // Get the selected signal
        SubGHzSignal *signal = &SIGNAL_CATEGORIES[menu.getSelectedCategory()].signals[menu.getSelectedSignal()];
        // Draw transmitting screen
        display.drawTransmitting(signal->name, signal->frequency);
        // Transmit the signal
        radio.transmit(signal->samples, signal->length, signal->frequency);
        // Return to the DETAILS screen
        menu.setCurrentScreen(MenuScreen::DETAILS);
    }

    display.show();
    delay(10);
}