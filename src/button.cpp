#include "button.h"

void Button::init() {
    pinMode(pin, INPUT_PULLUP);
    lastState = HIGH;
    lastDebounceTime = 0;
    clicked = false;
}

bool Button::pressed() {
    bool reading = digitalRead(pin);

    if (reading != lastState) {
        lastDebounceTime = millis();
    }

    lastState = reading;

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // Button is considered stable
        if (reading == LOW && !clicked) {
            clicked = true;
            return true;
        }
        if (reading == HIGH) {
            clicked = false;
        }
    }

    return false;
}

bool Button::held() {
    return  (digitalRead(pin) == LOW);
}
