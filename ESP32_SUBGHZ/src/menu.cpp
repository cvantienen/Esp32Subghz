#include "menu.h"

// =============================================================================
// SET COUNTS
// =============================================================================

void Menu::setCategoryCount(int8_t count) {
    categoryCount = count;
}

void Menu::setSignalCount(int8_t count) {
    signalCount = count;
}

// Set the current screen
void Menu::setCurrentScreen(MenuScreen state) {
    currentScreen = state;
}

// =============================================================================
// CATEGORY NAVIGATION
// =============================================================================

void Menu::categoryUp() {
    categoryIndex--;
    // Wrap: if we go below 0, jump to last category
    if (categoryIndex < 0) {
        categoryIndex = categoryCount - 1;
    }
}

void Menu::categoryDown() {
    categoryIndex++;
    // Wrap: if we exceed max, jump to first category
    if (categoryIndex >= categoryCount) {
        categoryIndex = 0;
    }
}

// =============================================================================
// SIGNAL NAVIGATION
// =============================================================================

void Menu::signalUp() {
    signalIndex--;
    if (signalIndex < 0) {
        signalIndex = signalCount - 1;
    }
}

void Menu::signalDown() {
    signalIndex++;
    if (signalIndex >= signalCount) {
        signalIndex = 0;
    }
}

// =============================================================================
// RESET
// =============================================================================

// Reset signal index to 0 when entering a new category.
void Menu::resetSignal() {
    signalIndex = 0;
}

// =============================================================================
// GETTERS
// =============================================================================

int8_t Menu::getSelectedCategory() const {
    return categoryIndex;
}

int8_t Menu::getSelectedSignal() const {
    return signalIndex;
}

// Get the current screen
MenuScreen Menu::getCurrentScreen() const {
    return currentScreen;
}

// get signal count
int8_t Menu::getSignalCount() const {
    return signalCount;
}

// =============================================================================
// PREV/NEXT FOR DISPLAY
// =============================================================================
// These calculate which items appear above/below the current selection.
// Used by the display to show 3 items at once for context.

int8_t Menu::getCategoryPrev() const {
    return (categoryIndex - 1 < 0) ? categoryCount - 1 : categoryIndex - 1;
}

int8_t Menu::getCategoryNext() const {
    return (categoryIndex + 1 >= categoryCount) ? 0 : categoryIndex + 1;
}

int8_t Menu::getSignalPrev() const {
    return (signalIndex - 1 < 0) ? signalCount - 1 : signalIndex - 1;
}

int8_t Menu::getSignalNext() const {
    return (signalIndex + 1 >= signalCount) ? 0 : signalIndex + 1;
}