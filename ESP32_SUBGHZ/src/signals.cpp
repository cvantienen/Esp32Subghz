#include "signals.h"

// ==================== SAMPLE DATA ARRAYS (STORED IN FLASH) ====================
// PROGMEM stores these in flash memory instead of RAM
// ESP32 reads PROGMEM automatically - no special read functions needed

const int16_t samples_touchtunes_p1[] PROGMEM = {
    250, -250, 250, -750, 750, -250, 250, -250
};

const int16_t samples_touchtunes_p2[] PROGMEM = {
    300, -300, 300, -800, 800, -300, 300, -300
};

const int16_t samples_touchtunes_p3[] PROGMEM = {
    275, -275, 275, -725, 725, -275, 275, -275
};

const int16_t samples_car_unlock[] PROGMEM = {
    400, -1200, 400, -400, 1200, -400
};

const int16_t samples_car_lock[] PROGMEM = {
    400, -1200, 400, -400, 1200, -400
};

const int16_t samples_car_trunk[] PROGMEM = {
    450, -1350, 450, -450, 1350, -450
};

const int16_t samples_garage_door1[] PROGMEM = {
    350, -1050, 350, -350, 1050, -350
};

const int16_t samples_garage_door2[] PROGMEM = {
    360, -1080, 360, -360, 1080, -360
};

// ==================== SIGNAL MENU STRUCTURE ====================

SubGHzSignal TOUCHTUNES_SIGNALS[] = {
    {"TT: P1", samples_touchtunes_p1, LENGTH_SAMPLES_TT_P1, 433.92},
    {"TT: P2", samples_touchtunes_p2, LENGTH_SAMPLES_TT_P2, 433.92},
    {"TT: P3", samples_touchtunes_p3, LENGTH_SAMPLES_TT_P3, 433.92}
};
const uint8_t NUM_TOUCHTUNES = sizeof(TOUCHTUNES_SIGNALS) / sizeof(SubGHzSignal);

SubGHzSignal CAR_SIGNALS[] = {
    {"Unlock", samples_car_unlock, LENGTH_SAMPLES_CAR_UNLOCK, 315.00},
    {"Lock", samples_car_lock, LENGTH_SAMPLES_CAR_LOCK, 315.00},
    {"Trunk", samples_car_trunk, LENGTH_SAMPLES_CAR_TRUNK, 315.00}
};
const uint8_t NUM_CAR = sizeof(CAR_SIGNALS) / sizeof(SubGHzSignal);

SubGHzSignal GARAGE_SIGNALS[] = {
    {"Door 1", samples_garage_door1, LENGTH_SAMPLES_GARAGE_DOOR1, 433.92},
    {"Door 2", samples_garage_door2, LENGTH_SAMPLES_GARAGE_DOOR2, 433.92}
};
const uint8_t NUM_GARAGE = sizeof(GARAGE_SIGNALS) / sizeof(SubGHzSignal);

// ==================== TOP-LEVEL CATEGORIES ====================

SubghzSignalList SIGNAL_CATEGORIES[] = {
    {"TouchTunes", TOUCHTUNES_SIGNALS, NUM_TOUCHTUNES},
    {"Cars", CAR_SIGNALS, NUM_CAR},
    {"Garage", GARAGE_SIGNALS, NUM_GARAGE}
};

const uint8_t NUM_OF_CATEGORIES = sizeof(SIGNAL_CATEGORIES) / sizeof(SubghzSignalList);