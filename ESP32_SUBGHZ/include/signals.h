#ifndef SIGNALS_H
#define SIGNALS_H

#include <Arduino.h>
#include <pgmspace.h>

// ==================== STRUCT DEFINITIONS ====================

struct SubGHzSignal {
    const char* name;           // String stored in flash
    const int16_t* samples;     // Pointer to PROGMEM array
    uint16_t length;
    float frequency;
};

struct SubghzSignalList {
    const char* name;
    SubGHzSignal* signals;
    uint8_t count;
};

// ==================== ARRAY LENGTH CONSTANTS ====================

// Using constexpr: type-safe, scoped, and works for array sizes

constexpr uint16_t LENGTH_SAMPLES_TT_P1 = 8;
constexpr uint16_t LENGTH_SAMPLES_TT_P2 = 8;
constexpr uint16_t LENGTH_SAMPLES_TT_P3 = 8;
constexpr uint16_t LENGTH_SAMPLES_CAR_UNLOCK = 6;
constexpr uint16_t LENGTH_SAMPLES_CAR_LOCK = 6;
constexpr uint16_t LENGTH_SAMPLES_CAR_TRUNK = 6;
constexpr uint16_t LENGTH_SAMPLES_GARAGE_DOOR1 = 6;
constexpr uint16_t LENGTH_SAMPLES_GARAGE_DOOR2 = 6;

// ==================== EXTERN DECLARATIONS ====================

// Sample arrays (stored in PROGMEM/flash)
extern const int16_t samples_touchtunes_p1[] PROGMEM;
extern const int16_t samples_touchtunes_p2[] PROGMEM;
extern const int16_t samples_touchtunes_p3[] PROGMEM;
extern const int16_t samples_car_unlock[] PROGMEM;
extern const int16_t samples_car_lock[] PROGMEM;
extern const int16_t samples_car_trunk[] PROGMEM;
extern const int16_t samples_garage_door1[] PROGMEM;
extern const int16_t samples_garage_door2[] PROGMEM;

// Signal arrays
extern SubGHzSignal TOUCHTUNES_SIGNALS[];
extern SubGHzSignal CAR_SIGNALS[];
extern SubGHzSignal GARAGE_SIGNALS[];

// Counts
extern const uint8_t NUM_TOUCHTUNES;
extern const uint8_t NUM_CAR;
extern const uint8_t NUM_GARAGE;

// Categories
extern SubghzSignalList SIGNAL_CATEGORIES[];
extern const uint8_t NUM_OF_CATEGORIES;

#endif