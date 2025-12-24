#ifndef DISPLAY_H
#define DISPLAY_H

#include "icon.h"
#include "generated_signals.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "animation.h"

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
    
    void drawAnimation(Animation &anim); 
  
    void drawCategoryMenu(int selected, int previous, int next,
                          int totalCategories);

    void drawSignalMenu(const char *categoryName, SubGHzSignal *signals,
                        int selected, int previous, int next, int totalSignals);

    void drawSignalDetails(const char *categoryName, SubGHzSignal *signal);

    void drawTransmitting(const char *signalName, float frequency);

    void drawAnimationFixedSize(Animation &anim, int y, int x, int width, int height);
};


#endif
