#ifndef DISPLAY_H
#define DISPLAY_H

#include "icon.h"
#include "signals.h"
#include <U8g2lib.h>
#include <Wire.h>

// ============================================================================
class OledDisplay {
  private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;
    const unsigned char **icons;

  public:
    OledDisplay(const unsigned char **iconArray);

    void init();
    void clear();
    void show();

    void drawIntroScreen();

    // OTA Display methods
    void drawOTAScreen(const char *ip, const char *status);
    void drawProgressBar(const char *label, uint8_t progress);
    void drawCenteredText(const char *line1, const char *line2);

    // ... existing code ...

    void drawCategoryMenu(int selected, int previous, int next,
                          int totalCategories);

    void drawSignalMenu(const char *categoryName, SubGHzSignal *signals,
                        int selected, int previous, int next, int totalSignals);

    void drawSignalDetails(const char *categoryName, SubGHzSignal *signal);

    void drawTransmitting(const char *signalName, float frequency);
};

// ============================================================================
// Draw helper Object Declaration
// ============================================================================
#endif
