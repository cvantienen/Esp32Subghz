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
private:
    int pin;
    int clicked = 0;

public:
    Button(int pinNumber) : pin(pinNumber) {}

    void init();
    bool pressed();
};

#endif // BUTTON_H
