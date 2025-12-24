#ifndef GENERATED_SIGNALS_H
#define GENERATED_SIGNALS_H

#include <pgmspace.h>
#include <Arduino.h>
// ==================== STRUCT DEFINITIONS ====================

struct SubGHzSignal {
    const char *name;       // String stored in flash
    const char *desc;       // Description stored in flash
    const int16_t *samples; // Pointer to PROGMEM array
    uint16_t length;
    float frequency;
};

struct SubghzSignalList {
    const char *name;
    SubGHzSignal *signals;
    uint8_t count;
};

// ==================== ARRAY LENGTH CONSTANTS ====================

constexpr uint16_t LENGTH_SAMPLES_TESLA_TESLA_CHARGE_PORT_OPENER_V1 = 1180;
constexpr uint16_t LENGTH_SAMPLES_TESLA_TESLA_CHARGE_PORT_OPENER_V2 = 1180;

constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_F1_RESTART = 17340;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MUSIC_VOL_ZONE_2UP = 17340;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MIC_VOL_MINUS_DOWN_ARROW = 17340;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MUSIC_VOL_ZONE_3DOWN = 17340;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_PAUSE = 16830;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MUSIC_VOL_ZONE_3UP = 17340;

constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MUSIC_VOL_ZONE_2DOWN = 17340;

constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MUSIC_VOL_ZONE_1UP = 17340;


constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_P3_SKIP = 17340;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_ON_OFF = 17340;

constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESBRUTE_MUSIC_VOL_ZONE_1DOWN = 17340;


constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_F1_RESTART = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_9 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_F3_MIC_A_MUTE = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_B_RIGHT_ARROW = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_1 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_F2_KEY = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_2 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_VOL_ZONE_2UP = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MIC_VOL_MINUS_DOWN_ARROW = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_VOL_ZONE_3DOWN = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_PAUSE = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_F4_MIC_B_MUTE = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_6 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_A_LEFT_ARROW = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_VOL_ZONE_3UP = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_5 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_OK = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_VOL_ZONE_2DOWN = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_3 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_0 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_P1 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MIC_VOL_PLUS_UP_ARROW = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_LOCK_QUEUE = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_VOL_ZONE_1UP = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_7 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_8 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_P2_EDIT_QUEUE = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_P3_SKIP = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_ON_OFF = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_SIG_4 = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_VOL_ZONE_1DOWN = 67;
constexpr uint16_t LENGTH_SAMPLES_TOUCHTUNESPIN_MUSIC_KARAOKE_STAR = 67;

// ==================== EXTERN DECLARATIONS ====================

extern const int16_t samples_tesla_tesla_charge_port_opener_v1[] PROGMEM;
extern const int16_t samples_tesla_tesla_charge_port_opener_sub_v2[] PROGMEM;

extern const int16_t samples_touchtunesbrute_f1_restart[] PROGMEM;
extern const int16_t samples_touchtunesbrute_music_vol_zone_2up[] PROGMEM;
extern const int16_t samples_touchtunesbrute_mic_vol_minus_down_arrow[] PROGMEM;
extern const int16_t samples_touchtunesbrute_music_vol_zone_3down[] PROGMEM;
extern const int16_t samples_touchtunesbrute_pause[] PROGMEM;

extern const int16_t samples_touchtunesbrute_music_vol_zone_3up[] PROGMEM;
extern const int16_t samples_touchtunesbrute_music_vol_zone_2down[] PROGMEM;
extern const int16_t samples_touchtunesbrute_mic_vol_plus_up_arrow[] PROGMEM;

extern const int16_t samples_touchtunesbrute_music_vol_zone_1up[] PROGMEM;

extern const int16_t samples_touchtunesbrute_p3_skip[] PROGMEM;
extern const int16_t samples_touchtunesbrute_on_off[] PROGMEM;

extern const int16_t samples_touchtunesbrute_music_vol_zone_1down[] PROGMEM;


extern const int16_t samples_touchtunespin_f1_restart[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_9[] PROGMEM;
extern const int16_t samples_touchtunespin_f3_mic_a_mute[] PROGMEM;
extern const int16_t samples_touchtunespin_b_right_arrow[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_1[] PROGMEM;
extern const int16_t samples_touchtunespin_f2_key[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_2[] PROGMEM;
extern const int16_t samples_touchtunespin_music_vol_zone_2up[] PROGMEM;
extern const int16_t samples_touchtunespin_mic_vol_minus_down_arrow[] PROGMEM;
extern const int16_t samples_touchtunespin_music_vol_zone_3down[] PROGMEM;
extern const int16_t samples_touchtunespin_pause[] PROGMEM;
extern const int16_t samples_touchtunespin_f4_mic_b_mute[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_6[] PROGMEM;
extern const int16_t samples_touchtunespin_a_left_arrow[] PROGMEM;
extern const int16_t samples_touchtunespin_music_vol_zone_3up[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_5[] PROGMEM;
extern const int16_t samples_touchtunespin_ok[] PROGMEM;
extern const int16_t samples_touchtunespin_music_vol_zone_2down[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_3[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_0[] PROGMEM;
extern const int16_t samples_touchtunespin_p1[] PROGMEM;
extern const int16_t samples_touchtunespin_mic_vol_plus_up_arrow[] PROGMEM;
extern const int16_t samples_touchtunespin_lock_queue[] PROGMEM;
extern const int16_t samples_touchtunespin_music_vol_zone_1up[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_7[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_8[] PROGMEM;
extern const int16_t samples_touchtunespin_p2_edit_queue[] PROGMEM;
extern const int16_t samples_touchtunespin_p3_skip[] PROGMEM;
extern const int16_t samples_touchtunespin_on_off[] PROGMEM;
extern const int16_t samples_touchtunespin_sig_4[] PROGMEM;
extern const int16_t samples_touchtunespin_music_vol_zone_1down[] PROGMEM;
extern const int16_t samples_touchtunespin_music_karaoke_star[] PROGMEM;

extern SubGHzSignal TESLA_SIGNALS[];
extern const uint8_t NUM_TESLA;

extern SubGHzSignal TOUCHTUNESBRUTE_SIGNALS[];
extern const uint8_t NUM_TOUCHTUNESBRUTE;

extern SubGHzSignal TOUCHTUNESPIN_SIGNALS[];
extern const uint8_t NUM_TOUCHTUNESPIN;


extern SubghzSignalList SIGNAL_CATEGORIES[];
extern const uint8_t NUM_OF_CATEGORIES;

#endif