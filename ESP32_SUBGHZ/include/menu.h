#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

// =============================================================================
// MENU CLASS
// =============================================================================
// Manages navigation indices for a two-level menu:
//   Level 1: Categories (TouchTunes, Cars, Garage, etc.)
//   Level 2: Signals within each category
//
// This class handles:
//   - Index tracking with wrap-around (go past end → back to start)
//   - Calculating prev/next indices for 3-item display

// Menu Screen States
// =============================================================================
// The menu is a state machine with 4 screens.
enum class MenuScreen : uint8_t {
    CATEGORIES,  // List of signal categories
    SIGNALS,     // Signals within selected category
    DETAILS,     // Info about selected signal  
    TRANSMIT     // Sending signal
};

class Menu {
private:
    MenuScreen currentScreen = MenuScreen::CATEGORIES;
    int8_t categoryIndex = 0;   // Currently selected category (0 to categoryCount-1)
    int8_t signalIndex = 0;     // Currently selected signal within category
    int8_t categoryCount;   // Total number of categories
    int8_t signalCount;     // Total signals in current category

public:

    // Constructor - the MenuScreen object is initialized to CATEGORIES screen
    Menu() {
        currentScreen = MenuScreen::CATEGORIES;
    }

    // =============================================================================
    // SET COUNTS - Call these to tell Menu how many items exist
    // -------------------------------------------------------------------------
    void setCategoryCount(int8_t count);  // Call once in setup()
    void setSignalCount(int8_t count);    // Call when entering a category (count varies per category)
    // Set the current screen
    void setCurrentScreen(MenuScreen state); 
    
    // -------------------------------------------------------------------------
    // NAVIGATION - Move selection up/down with wrap-around
    // -------------------------------------------------------------------------
    void categoryUp();    // Move to previous category (wraps to last if at first)
    void categoryDown();  // Move to next category (wraps to first if at last)
    void signalUp();      // Move to previous signal
    void signalDown();    // Move to next signal

    // -------------------------------------------------------------------------
    // RESET SIGNAL - Call when entering a new category
    void resetSignal();

    // -------------------------------------------------------------------------
    // GETTERS - Get current selection indices
    // -------------------------------------------------------------------------
    int8_t getSelectedCategory() const;
    int8_t getSelectedSignal() const;
    // Get the current screen
    MenuScreen getCurrentScreen() const;
    // get signal count
    int8_t getSignalCount() const;
    // get category count
    int8_t getCategoryCount() const;
    // -------------------------------------------------------------------------
    // PREV/NEXT - For displaying 3 items at once (prev, current, next)
    // -------------------------------------------------------------------------
    // The display shows the previous item, current selection, and next item.
    // These methods calculate those indices with proper wrap-around.
    int8_t getCategoryPrev() const;
    int8_t getCategoryNext() const;
    int8_t getSignalPrev() const;
    int8_t getSignalNext() const;
};

#endif