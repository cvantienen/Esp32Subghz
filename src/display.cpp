#include "display.h"


// ============================================================================
// DISPLAY CLASS - Handles all screen rendering
// ============================================================================

// Constructor initializes the internal display object
// You must match your specific display constructor!
OledDisplay::OledDisplay(const unsigned char **iconArray)
    : display(U8G2_R0, U8X8_PIN_NONE), // YOUR DISPLAY CONSTRUCTOR HERE
      icons(iconArray) {}

void OledDisplay::init() {
    display.setColorIndex(1);
    display.begin();
    display.clearBuffer();
}

void OledDisplay::clear() { display.clearBuffer(); }

void OledDisplay::show() { display.sendBuffer(); } 

// ----------------------------------------------------------
// Draw intro screen
// ----------------------------------------------------------
void OledDisplay::drawIntroScreen() {
    display.setFont(u8g_font_7x14);
    display.drawXBM(0, 0, 128, 64, intro_bitmap);
}

// ----------------------------------------------------------
// Draw category menu screen
// ----------------------------------------------------------

void OledDisplay::drawCategoryMenu(int selected, int previous, int next,
                                   int totalCategories) {                            
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
    display.drawBox(125, 64 / totalCategories * selected, 3,
                    64 / totalCategories);

    // Bobbing signature
    display.setFont(u8g2_font_5x8_tf);
    int bob = (int)(sin(millis() / 300.0) * 3); // Bob ±3 pixels
    display.drawXBMP(108, 45 + bob, 16, 16, mini_ghost_bitmap);

    //
}

// ----------------------------------------------------------
// Draw signal submenu
// ----------------------------------------------------------
void OledDisplay::drawSignalMenu(const char *categoryName,
                                 SubGHzSignal *signals, int selected,
                                 int previous, int next, int totalSignals) {

    // ─── HEADER BAR ───────────────────────────────────────
    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(4, 9, categoryName);
    display.drawHLine(0, 12, 120);

    // ─── PREVIOUS ITEM (dimmed, smaller) ──────────────────
    display.setFont(u8g2_font_5x8_tf);
    display.drawStr(6, 22, signals[previous].name);

    // ─── SELECTED ITEM CARD ───────────────────────────────
    // Rounded selection box
    display.drawRFrame(2, 25, 118, 22, 3);

    // Selected signal name (bold)
    display.setFont(u8g2_font_7x13B_tf);
    display.drawStr(8, 37, signals[selected].name);

    // Description (smaller, under name)
    display.setFont(u8g2_font_5x7_tf);
    display.drawStr(8, 45, signals[selected].desc);

    // Small selection indicator
    display.drawTriangle(120, 33, 116, 30, 116, 36);

    // ─── NEXT ITEM (dimmed, smaller) ──────────────────────
    display.setFont(u8g2_font_5x8_tf);
    display.drawStr(6, 58, signals[next].name);

    // ─── SCROLLBAR ────────────────────────────────────────
    // Track
    display.drawVLine(125, 14, 48);

    // Thumb position
    int thumb_y = 14;
    if (totalSignals > 1) {
        thumb_y = map(selected, 0, totalSignals - 1, 14, 56);
    }
    display.drawBox(124, thumb_y, 3, 6);
}

// ----------------------------------------------------------
// Draw signal details
// ----------------------------------------------------------
void OledDisplay::drawSignalDetails(const char *categoryName,
                                    SubGHzSignal *signal) {
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
void OledDisplay::drawTransmitting(const char *signalName, float frequency) {
    display.setFont(u8g2_font_9x15B_tf);
    display.drawStr(15, 25, "SENDING...");

    display.setFont(u8g2_font_7x13_tf);
    display.drawStr(10, 45, signalName);

    char freq_str[20];
    snprintf(freq_str, 20, "%.2f MHz", frequency);
    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(30, 58, freq_str);
}


void OledDisplay::drawAnimation(Animation &anim) {
    // Get the current frame from the animation object (passed by reference)
    const unsigned char* currentFrame = anim.getCurrentFrame();
    display.drawXBMP(0, // fullscreen animation 
        0,              // fullscreen animation 
        128,            // fullscreen animation 
        64,             // fullscreen animation 
        currentFrame); // Bitmap
    anim.updateAnimation();// update animation frame
}


