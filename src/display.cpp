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

    display.setFont(u8g_font_7x13);
    display.drawStr(25, 15, SIGNAL_CATEGORIES[previous].name);
    display.drawXBMP(4, 2, 16, 16, icons[previous % 8]);

    display.setFont(u8g_font_7x13B);
    display.drawStr(25, 37, SIGNAL_CATEGORIES[selected].name);
    display.drawXBMP(4, 24, 16, 16, icons[selected % 8]);

    display.setFont(u8g_font_7x13);
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

// ═══════════════════════════════════════════════════════════
//  SIGNAL DETAILS SCREEN
// ═══════════════════════════════════════════════════════════

void OledDisplay::drawSignalMenu(const char *categoryName,
                                 SubGHzSignal *signals, int selected,
                                 int previous, int next, int totalSignals) {

    // ═══════════════════════════════════════════════════════
    //  HEADER BAR
    // ═══════════════════════════════════════════════════════
    display.setFont(u8g2_font_7x13B_tf);
    display.drawStr(4, 10, categoryName);
    
    // Double line separator
    display.drawHLine(0, 13, 128);
    display.drawHLine(0, 14, 128);

    // ═══════════════════════════════════════════════════════
    //  PREVIOUS ITEM (dimmed preview)
    // ═══════════════════════════════════════════════════════
    display.setFont(u8g2_font_5x8_tf);
    display.drawStr(10, 24, signals[previous].name);

    // ═══════════════════════════════════════════════════════
    //  SELECTED ITEM CARD (highlighted)
    // ═══════════════════════════════════════════════════════
    // Double rounded frame for emphasis
    display.drawRFrame(1, 27, 120, 24, 4);
    
    // Selected signal name (bold, larger)
    display.setFont(u8g2_font_7x13_tf);
    display.drawStr(8, 38, signals[selected].name);
    
    // Description text
    display.setFont(u8g2_font_5x7_tf);
    display.drawStr(8, 47, signals[selected].desc);
    
    // ═══════════════════════════════════════════════════════
    //  NEXT ITEM (dimmed preview)
    // ═══════════════════════════════════════════════════════
    display.setFont(u8g2_font_5x8_tf);
    display.drawStr(10, 60, signals[next].name);

    // ═══════════════════════════════════════════════════════
    //  SCROLLBAR (modern style)
    // ═══════════════════════════════════════════════════════
    // Scrollbar track
    display.drawVLine(126, 15, 48);
    
    // Calculate thumb position
    int thumb_y = 15;
    if (totalSignals > 1) {
        thumb_y = map(selected, 0, totalSignals - 1, 15, 55);
    }
    
    // Rounded scrollbar thumb
    display.drawRBox(125, thumb_y, 3, 8, 1);

    // ═══════════════════════════════════════════════════════
    //  FOOTER INFO (item counter)
    // ═══════════════════════════════════════════════════════

    char positionStr[10];
    snprintf(positionStr, sizeof(positionStr), "%d/%d", selected + 1, totalSignals);
    
    display.setFont(u8g2_font_4x6_tf);
    int strWidth = display.getStrWidth(positionStr);
    display.drawStr(126 - strWidth, 63, positionStr);
}


// ═══════════════════════════════════════════════════════════
//  SIGNAL DETAILS SCREEN
// ═══════════════════════════════════════════════════════════

void OledDisplay::drawSignalDetails(const char *categoryName, SubGHzSignal *signal) {
    // ──────────────────────────────────────────────────────────────────
    //  HEADER WITH SIGNAL NAME
    // ──────────────────────────────────────────────────────────────────
    display.setFont(u8g2_font_7x13B_tf);
    display.drawStr(4, 10, categoryName);
    

    // ──────────────────────────────────────────────────────────────────
    //  INFORMATION CARD
    // ──────────────────────────────────────────────────────────────────
    // Card background with rounded corners (slightly larger to add space)
    display.drawRFrame(3, 19, 122, 35, 6);  // Slightly taller card for better spacing
    display.setFont(u8g2_font_6x10_tf);

    // Signal name (larger font, bold)
    char name_str[30];
    snprintf(name_str, sizeof(name_str), "%s", signal->name);  // Using snprintf for safety
    display.drawStr(7, 29, name_str);  // Positioned better for alignment
    display.drawHLine(7, 31, 64);  // Separator line below name
    
    // Signal description (smaller font)
    display.setFont(u8g2_font_5x7_tf);
    display.drawStr(7, 40, "Description:");
    display.drawStr(7, 48, signal->desc);  // Position description below name

    // ──────────────────────────────────────────────────────────────────
    //  FOOTER INSTRUCTION (inverted bar)
    // ──────────────────────────────────────────────────────────────────
    display.setDrawColor(1);  // Set draw color to normal
    display.drawRBox(2, 57, 126, 10, 3);  // Button-like box for instructions

    // Inverted bar (to make it stand out)
    display.setDrawColor(0);  // Inverted text
    int footerTextWidth = display.getStrWidth("SELECT to transmit");
    display.drawStr((128 - footerTextWidth) / 2, 63, "SELECT to transmit");

    // Reset to normal text color
    display.setDrawColor(1);  
}

// ═══════════════════════════════════════════════════════════════════
//  TRANSMITTING SCREEN
// ═══════════════════════════════════════════════════════════════════
void OledDisplay::drawTransmitting(const char *signalName, float frequency) {
    
    // ──────────────────────────────────────────────────────────────────
    //  STATUS CARD WITH DOUBLE FRAME
    // ──────────────────────────────────────────────────────────────────
    
    display.drawRFrame(1, 1, 127, 63, 5);

    // ──────────────────────────────────────────────────────────────────
    //  "SENDING..." TEXT (centered, bold)
    // ──────────────────────────────────────────────────────────────────
    
    display.setFont(u8g2_font_6x10_tf);
    const char* statusText = "SENDING...";
    int statusWidth = display.getStrWidth(statusText);
    display.drawStr((128 - statusWidth) / 2, 16, statusText);
    display.drawHLine(18, 26, 92);
    // ──────────────────────────────────────────────────────────────────
    //  ANIMATION SPACE (add your animation here)
    // ──────────────────────────────────────────────────────────────────
    
    // Reserved space: y = 28 to y = 35 (center area)
    // Animation suggestions:
    // - Small spinning icon (centered at x=60, y=32)
    // - Pulsing dot at x=60, y=32
    // - Signal bars at x=20, y=32 (with vertical lines)
    
    // ──────────────────────────────────────────────────────────────────
    //  SIGNAL INFO (name + frequency)
    // ──────────────────────────────────────────────────────────────────
    // Thin separator line

    
    // Signal name (centered, bold)
    display.setFont(u8g2_font_6x10_tf);
    int nameWidth = display.getStrWidth(signalName);
    display.drawStr((128 - nameWidth) / 2, 56, signalName);

}


// ═══════════════════════════════════════════════════════════
//  FULLSCREEN ANIMATION HELPER
// ═══════════════════════════════════════════════════════════

void OledDisplay::drawAnimation(Animation &anim) {
    // Get current frame from animation object
    const unsigned char* currentFrame = anim.getCurrentFrame();
    
    // Draw fullscreen bitmap (128x64)
    display.drawXBMP(0, 0, 128, 64, currentFrame);
    
    // Update to next frame
    anim.updateAnimation();
}



