#ifndef RADIO_H
#define RADIO_H

// RADIO OBJECT
class SubghzRadio {

  private:
    // Pin definitions
    static constexpr int PIN_SCK  = 18;
    static constexpr int PIN_MISO = 19;
    static constexpr int PIN_MOSI = 23;
    static constexpr int PIN_SS   = 5;
    static constexpr int PIN_GDO0 = 2;
    static constexpr int PIN_GDO2 = 4;
    


    // Radio settings
    float frequency = 433.92;// MHz
    int dataRate = 512;// bps
    int modulation = 2;// 2=FSK
    int pktFormat = 3;// 3=Normal mode
    
    // Radio mode state
    enum class Mode { TX, RX, IDLE, SLEEP };
    Mode currentMode; // Current radio mode

  public:
    
    // Setters
    void setFrequency(float mhz);
    void setDataRate(int rate);
    void setModulation(int mod);
    void setPacketFormat(int format);
    void setRadioMode(Mode mode);

    // Getters
    float getFrequency();
    int getDataRate();
    int getModulation();
    int getPacketFormat();
    Mode getMode();
    
    
    // Transmit samples
    void transmit(const int16_t* samples, uint16_t samplesLength, float mhz);
    // Initialize CC1101
    void initCC1101();
};

#endif
