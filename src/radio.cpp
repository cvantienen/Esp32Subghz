#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <radio.h>
#include "esp_task_wdt.h"

// ---------------------------
// CC1101 INITIALIZATION
// ---------------------------
void SubghzRadio::initCC1101(float mhz) {
    Serial.println("[initCC1101] Starting CC1101 init...");
    ELECHOUSE_cc1101.setSpiPin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_SS);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setGDO(PIN_GDO0, PIN_GDO2);
    ELECHOUSE_cc1101.setMHZ(mhz);
    ELECHOUSE_cc1101.SetTx();
    ELECHOUSE_cc1101.setModulation(2);  // ASK/OOK
    ELECHOUSE_cc1101.setDRate(512);
    ELECHOUSE_cc1101.setPktFormat(3);  
    
    if (!ELECHOUSE_cc1101.getCC1101()) {
        Serial.println("[initCC1101] ERROR: CC1101 Connection Failed!");
        return;
    }
    
    Serial.println("[initCC1101] ✅ CC1101 Initialized for RAW replay");
    delay(50);
}

// ---------------------------
// FAST TRANSMIT - OPTIMIZED FOR SPEED
// ---------------------------
void SubghzRadio::transmit(const int16_t *samples, uint16_t samplesLength, float mhz) {
    if (!samples || samplesLength == 0) {
        Serial.println("[transmit] ERROR: Invalid samples");
        return;
    }
    
    Serial.println("[transmit] ========================================");
    Serial.print("[transmit] Frequency: ");
    Serial.print(mhz, 2);
    Serial.println(" MHz");
    Serial.print("[transmit] Samples: ");
    Serial.println(samplesLength);
    
    SubghzRadio::initCC1101(mhz);
    
    Serial.println("[transmit] Transmitting...");
    
    unsigned long startTime = micros();
    
    // Transmit all samples as fast as possible
    for (uint16_t i = 0; i < samplesLength; i++) {
        int16_t duration = samples[i];
        bool signalLevel = (duration >= 0) ? 1 : 0;
        
        if (duration < 0) duration = -duration;
        if (duration == 0) duration = 1;
        
        digitalWrite(PIN_GDO0, signalLevel);
        delayMicroseconds(duration);
    }
    
    digitalWrite(PIN_GDO0, LOW);
    
    unsigned long totalTime = micros() - startTime;
    
    Serial.print("[transmit] ✅ Complete in ");
    Serial.print(totalTime / 1000.0);
    Serial.println(" ms");
    Serial.println("[transmit] ========================================");
}

// ---------------------------
// TRANSMIT WITH REPEATS
// ---------------------------
void SubghzRadio::transmitWithRepeats(const int16_t *samples, uint16_t samplesLength, 
                                     float mhz, uint8_t repeats) {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.print("║ Transmitting ");
    Serial.print(repeats);
    Serial.println(" times");
    Serial.println("╚════════════════════════════════════════╝");
    
    for (uint8_t i = 0; i < repeats; i++) {
        Serial.print("→ Repeat ");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.println(repeats);
        
        transmit(samples, samplesLength, mhz);
        
        if (i < repeats - 1) {
            vTaskDelay(10); // Short delay between repeats
        }
    }
    
    Serial.println("╚════════════════════════════════════════╝\n");
}

// ---------------------------
// BRUTE FORCE OPTIMIZED: TRANSMIT FROM PROGMEM WITH WDT SAFETY
// ---------------------------
void SubghzRadio::transmitFromProgmem(const int16_t *samples, uint16_t samplesLength, 
                                     float mhz, uint8_t repeats) {
    /*  CHUNK_SIZE: How many samples to play before resetting WDT
        Smaller = more WDT resets (safer but slower)
        Larger = fewer WDT resets (faster but riskier)
        each touch tunes singal is 67 samples long, for 
        the chunk will contain 20 of those signals
        IE: 67 * 20 = 1340 samples 
        this allows signals smaller then 1340 samples to be transmitted
        without splitting them further. Like the Tesala signals
    */
    const uint16_t CHUNK_SIZE = 1340;  // Reset WDT every 1340 samples
    
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.print("║ Signal Length: ");
    Serial.print(samplesLength);
    Serial.println(" samples");
    Serial.print("║ Frequency: ");
    Serial.print(mhz, 2);
    Serial.println(" MHz");
    Serial.print("║ Chunk Size: ");
    Serial.print(CHUNK_SIZE);
    Serial.println(" samples");
    Serial.print("║ Repeats: ");
    Serial.println(repeats);
    Serial.println("╚════════════════════════════════════════╝");
    
    SubghzRadio::initCC1101(mhz);
    
    for (uint8_t repeat = 0; repeat < repeats; repeat++) {
        if (repeats > 1) {
            Serial.print("→ Repeat ");
            Serial.print(repeat + 1);
            Serial.print("/");
            Serial.println(repeats);
        }
        
        unsigned long txStartTime = micros();
        uint16_t offset = 0;
        uint16_t chunkCount = 0;
        
        // Process signal in chunks
        while (offset < samplesLength) {
            chunkCount++;
            uint16_t chunkLen = min((uint16_t)CHUNK_SIZE, (uint16_t)(samplesLength - offset));
            
            // Load chunk from PROGMEM to RAM
            int16_t chunk[CHUNK_SIZE];
            for (uint16_t i = 0; i < chunkLen; i++) {
                chunk[i] = pgm_read_word(&samples[offset + i]);
            }
            
            // Transmit chunk AS FAST AS POSSIBLE (no yields inside)
            for (uint16_t i = 0; i < chunkLen; i++) {
                int16_t duration = chunk[i];
                bool signalLevel = (duration >= 0) ? 1 : 0;
                
                if (duration < 0) duration = -duration;
                if (duration == 0) duration = 1;
                
                digitalWrite(PIN_GDO0, signalLevel);
                delayMicroseconds(duration);
            }
            
            offset += chunkLen;
            
            // After each chunk: Reset WDT
            if (offset < samplesLength) {  // Don't delay after last chunk
                Serial.print("Chunk");
                Serial.print(chunkCount);
                esp_task_wdt_reset();
                vTaskDelay(10);
            }
        }
        
        digitalWrite(PIN_GDO0, LOW);
        
        unsigned long txTime = micros() - txStartTime;
        Serial.print("  ✅ Transmitted in ");
        Serial.print(txTime / 10000.0);
        Serial.print(" s (");
        Serial.print(chunkCount);
        Serial.println(" chunks)");
        
        // reset WDT between repeats
        if (repeat < repeats - 1) {
            esp_task_wdt_reset();
            yield();
        }
    }
    
    Serial.println("╚════════════════════════════════════════╝\n");
}

// ---------------------------
// BRUTE FORCE BATCH: Transmit multiple signals back-to-back
// ---------------------------
void SubghzRadio::transmitBatch(const SubGHzSignal signals[], uint16_t signalCount, 
                               uint8_t repeatsPerSignal) {
    Serial.println("\n╔═══════════════════════════════════════════════╗");
    Serial.print("║ BRUTE FORCE BATCH: ");
    Serial.print(signalCount);
    Serial.println(" signals");
    Serial.println("╚═══════════════════════════════════════════════╝");
    
    unsigned long batchStart = millis();
    
    for (uint16_t i = 0; i < signalCount; i++) {
        Serial.print("\n[");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.print(signalCount);
        Serial.print("] ");
        Serial.println(signals[i].name);
        
        transmitFromProgmem(signals[i].samples, signals[i].length, 
                          signals[i].frequency, repeatsPerSignal);
        
        // Reset WDT between signals
        esp_task_wdt_reset();
        yield();
        
    }
    
    unsigned long batchTime = millis() - batchStart;
    
    Serial.println("\n╔═══════════════════════════════════════════════╗");
    Serial.print("║ BATCH COMPLETE: ");
    Serial.print(batchTime / 1000.0);
    Serial.println(" seconds");
    Serial.print("║ Signals sent: ");
    Serial.println(signalCount);
    Serial.println("╚═══════════════════════════════════════════════╝\n");
}

// ---------------------------
// TRANSMIT SIGNAL STRUCTURE
// ---------------------------
void SubghzRadio::transmitSignal(const SubGHzSignal &signal, uint8_t repeats) {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.print("║ Signal: ");
    Serial.println(signal.name);
    Serial.print("║ Freq: ");
    Serial.print(signal.frequency, 2);
    Serial.println(" MHz");
    Serial.print("║ Length: ");
    Serial.print(signal.length);
    Serial.println(" samples");
    Serial.println("╚════════════════════════════════════════╝");
    
    transmitFromProgmem(signal.samples, signal.length, signal.frequency, repeats);
}

// ---------------------------
// TEST TRANSMISSION
// ---------------------------
void SubghzRadio::testTransmit(float mhz) {
    Serial.println("\n[TEST] ═══════════════════════════════════");
    Serial.println("[TEST] Running basic transmission test...");
    
    int16_t testPattern[20] = {
        1000, -1000, 1000, -1000, 1000, -1000, 1000, -1000, 1000, -1000,
        1000, -1000, 1000, -1000, 1000, -1000, 1000, -1000, 1000, -1000
    };
    
    transmitWithRepeats(testPattern, 20, mhz, 3);
    
    Serial.println("[TEST] ✅ Test complete");
    Serial.println("[TEST] ═══════════════════════════════════\n");
}

// ---------------------------
// USAGE EXAMPLE FOR BRUTE FORCE
// ---------------------------
/*
// Your signal array1340
const int16_t samples_touchtunespin_music_karaoke_star[] PROGMEM = {
    9056, -4528, 566, -566, 566, -1698, 566, -566,
    566, -1698, 566, -1698, 566, -1698, 566, -566,
    // ... rest of your data
};

// Define multiple signals for brute force
SubGHzSignal BRUTEFORCE_SIGNALS[] = {
    {"Code 1", "code1.sub", samples_code1, 67, 433.92},
    {"Code 2", "code2.sub", samples_code2, 67, 433.92},
    {"Code 3", "code3.sub", samples_code3, 67, 433.92},
    // ... hundreds more
};

void setup() {
    Serial.begin(115200);
    radio.initCC1101();
}

void loop() {
    // Method 1: Single signal, multiple repeats
    radio.transmitSignal(BRUTEFORCE_SIGNALS[0], 1);
    
    // Method 2: Batch of signals (BRUTE FORCE)
    radio.transmitBatch(BRUTEFORCE_SIGNALS, 100, 1);  // 100 signals, 1 repeat each
    
    delay(5000);
}
*/