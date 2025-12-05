#include "button.h"

void Button::init() {
    pinMode(pin, INPUT_PULLUP);
}

bool Button::pressed() {
    if (digitalRead(pin) == LOW && clicked == 0) {
        clicked = 1;
        return true;
    }
    if (digitalRead(pin) == HIGH) {
        clicked = 0;
    }
    return false;
}