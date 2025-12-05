#include "display.h"
// ============================================================================
// DISPLAY CLASS - Handles all screen rendering
// ============================================================================

// Constructor initializes the internal display object
// You must match your specific display constructor!
OledDisplay::OledDisplay(const unsigned char** iconArray)
    : display(U8G2_R0, U8X8_PIN_NONE),   // YOUR DISPLAY CONSTRUCTOR HERE
        icons(iconArray)
{}

void OledDisplay::init() {
    display.setColorIndex(1);
    display.begin();
    display.clearBuffer();
}

void OledDisplay::clear() {
    display.clearBuffer();
}

void OledDisplay::show() {
    display.sendBuffer();
}
// ----------------------------------------------------------
// Draw category menu screen
// ----------------------------------------------------------

void OledDisplay::drawCategoryMenu(int selected, int previous, int next, int totalCategories) {
    display.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline);

    display.setFont(u8g_font_7x14);
    display.drawStr(25, 15, SIGNAL_CATEGORIES[previous].name);
    display.drawXBMP(4, 2, 16, 16, icons[previous % 8]);

    display.setFont(u8g_font_7x14B);
    display.drawStr(25, 37, SIGNAL_CATEGORIES[selected].name);
    display.drawXBMP(4, 24, 16, 16, icons[selected % 8]);

    display.setFont(u8g_font_7x14);
    display.drawStr(25, 59, SIGNAL_CATEGORIES[next].name);
    display.drawXBMP(4, 46, 16, 16, icons[next % 8]);

    display.drawXBMP(128 - 8, 0, 8, 64, bitmap_scrollbar_background);
    display.drawBox(125, 64 / totalCategories * selected, 3, 64 / totalCategories);

    display.setFont(u8g2_font_5x8_tf);
    display.drawStr(108, 63, "C.J.");

    //
}

// ----------------------------------------------------------
// Draw signal submenu
// ----------------------------------------------------------
void OledDisplay::drawSignalMenu(const char* categoryName, SubGHzSignal* signals, 
                    int selected, int previous, int next, int totalSignals) {

    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(5, 10, categoryName);
    display.drawLine(0, 12, 128, 12);

    display.drawFrame(2, 18, 124, 18);

    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(8, 26, signals[previous].name);

    display.setFont(u8g2_font_7x13B_tf);
    display.drawStr(8, 46, signals[selected].name);

    display.setFont(u8g2_font_5x8_tf);
    char freq_str[20];
    snprintf(freq_str, 20, "%.2f MHz", signals[selected].frequency);
    display.drawStr(8, 56, freq_str);

    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(8, 66, signals[next].name);

    display.drawFrame(122, 14, 4, 48);
    int scrollbar_pos = map(selected, 0, totalSignals - 1, 0, 40);
    display.drawBox(123, 15 + scrollbar_pos, 2, 6);
}

// ----------------------------------------------------------
// Draw signal details
// ----------------------------------------------------------
void OledDisplay::drawSignalDetails(const char* categoryName, SubGHzSignal* signal) {
    display.setFont(u8g2_font_7x13B_tf);
    display.drawStr(5, 12, signal->name);
    display.drawLine(0, 14, 128, 14);

    display.setFont(u8g2_font_6x10_tf);

    char cat_str[25];
    snprintf(cat_str, 25, "Cat: %s", categoryName);
    display.drawStr(5, 28, cat_str);

    char freq_str[25];
    snprintf(freq_str, 25, "Freq: %.2f MHz", signal->frequency);
    display.drawStr(5, 40, freq_str);

    char len_str[25];
    snprintf(len_str, 25, "Samples: %d", signal->length);
    display.drawStr(5, 52, len_str);

    display.setFont(u8g2_font_5x8_tf);
    display.drawStr(5, 63, "SELECT to transmit");
}

// ----------------------------------------------------------
// Draw transmit screen
// ----------------------------------------------------------
void OledDisplay::drawTransmitting(const char* signalName, float frequency) {
    display.setFont(u8g2_font_9x15B_tf);
    display.drawStr(15, 25, "SENDING...");

    display.setFont(u8g2_font_7x13_tf);
    display.drawStr(10, 45, signalName);

    char freq_str[20];
    snprintf(freq_str, 20, "%.2f MHz", frequency);
    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(30, 58, freq_str);
}


