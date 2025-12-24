#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include "generated_signals.h"

// =============================================================================
// TRANSMIT REQUEST STRUCTURE (includes menu state)
// =============================================================================
struct TransmitRequest {
    int8_t category;
    int8_t signalIndex;
};

// RADIO OBJECT
class SubghzRadio {

  private:
    // Pin definitions
    static constexpr int PIN_SCK = 18;
    static constexpr int PIN_MISO = 19;
    static constexpr int PIN_MOSI = 23;
    static constexpr int PIN_SS = 5;
    static constexpr int PIN_GDO0 = 12;
    static constexpr int PIN_GDO2 = 4;

  public:
    // Setters
    void initCC1101(float mhz);
    // ---------------------------
    // TRANSMIT RAW SAMPLES (FLIPPER ZERO REPLAY)

    void transmit(const int16_t *samples, uint16_t samplesLength, float mhz);
    // ---------------------------
    // TRANSMIT WITH REPEATS (RECOMMENDED FOR REMOTES)
    // ---------------------------
    void transmitWithRepeats(const int16_t *samples, uint16_t samplesLength, 
                                        float mhz, uint8_t repeats); 
                                        
    void transmitBatch(const SubGHzSignal signals[], uint16_t signalCount, 
                               uint8_t repeatsPerSignal);
    // ---------------------------
    // TRANSMIT FROM PROGMEM (FOR YOUR FLIPPER ARRAYS)
    // ---------------------------
    void transmitFromProgmem(const int16_t *samples, uint16_t samplesLength, 
                                        float mhz, uint8_t repeats);
    // ---------------------------
    // TRANSMIT SIGNAL STRUCTURE (FOR YOUR SubGHzSignal ARRAYS)
    // ---------------------------
    void transmitSignal(const SubGHzSignal &signal, uint8_t repeats);
    // ---------------------------
    // TEST TRANSMISSION
    // ---------------------------
    void testTransmit(float mhz);
};

#endif
