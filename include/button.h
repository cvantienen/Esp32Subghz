#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

// Button pin constants
#define BUTTON_UP_PIN     32
#define BUTTON_SELECT_PIN 25
#define BUTTON_DOWN_PIN   27
#define BUTTON_BACK_PIN   14

// Button class to handle button states
class Button {
    int pin;
    bool clicked = false;
    bool lastState = HIGH;
    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 30; // 30 ms


public:
public:
    Button(int pinNumber) : pin(pinNumber) {}

    void init();
    bool pressed();   // fires once per click
    bool held();      // true while button held
};


#endif // BUTTON_H
