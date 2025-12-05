#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <radio.h>


// ---------------------------
// CC1101 INITIALIZATION
// ---------------------------

void SubghzRadio::initCC1101()
{
  Serial.println("[initCC1101] Starting CC1101 init...");
  ELECHOUSE_cc1101.setSpiPin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_SS);
  ELECHOUSE_cc1101.setGDO(PIN_GDO0, PIN_GDO2);
  ELECHOUSE_cc1101.Init();

  // Verify connection
  if (!ELECHOUSE_cc1101.getCC1101())
  {
    Serial.println("[initCC1101] -- CC1101 Connection Error!");
  }
  else
  {
    Serial.print("[initCC1101] -- CC1101 Initialized");
  }

  delay(50); // prevent brownout
}

// ---------------------------
// SETTERS: RADIO PARAMETERS
// ---------------------------

void SubghzRadio::setFrequency(float mhz)
{
  frequency = mhz;
  ELECHOUSE_cc1101.setMHZ(mhz);
}

void SubghzRadio::setDataRate(int rate)
{
  dataRate = rate;
  ELECHOUSE_cc1101.setDRate(rate);
}

void SubghzRadio::setModulation(int mod)
{
  modulation = mod;
  ELECHOUSE_cc1101.setModulation(mod);
}

void SubghzRadio::setPacketFormat(int format)
{
  pktFormat = format;
  ELECHOUSE_cc1101.setPktFormat(format);
}

void SubghzRadio::setRadioMode(Mode mode) {
    // 0=TX -- transmit
    // 1=RX -- receive
    // 2=IDLE -- idle
    // 3=SLEEP -- sleep
    if (mode == currentMode) {
        return; // No change needed
    }
    switch(mode) {
      case Mode::TX:
        ELECHOUSE_cc1101.SetTx();  // Enter transmit mode
        break;
      case Mode::RX:
        ELECHOUSE_cc1101.SetRx();  // Enter receive mode
        break;
      case Mode::IDLE:
        ELECHOUSE_cc1101.setSidle(); // Idle mode (optional)
        break;
      case Mode::SLEEP:
        ELECHOUSE_cc1101.goSleep(); // Sleep mode
        break;
    }
    currentMode = mode;
} 

// ---------------------------
// GETTERS: RADIO PARAMETERS
//  -------------------------

float SubghzRadio::getFrequency()
{
  return frequency;
}
int SubghzRadio::getDataRate()
{
  return dataRate;
}
int SubghzRadio::getModulation()
{
  return modulation;
}
int SubghzRadio::getPacketFormat()
{
  return pktFormat;
}
SubghzRadio::Mode SubghzRadio::getMode() 
{
  return currentMode;
} 


// ---------------------------
// TRANSMIT SAMPLES 
// ---------------------------
void SubghzRadio::transmit(const int16_t* samples, uint16_t samplesLength, float mhz)
{
  setFrequency(mhz);
  Serial.print("[transmit] Transmitting ");
  Serial.print(samplesLength);
  Serial.println(" samples...");

  int delayTime = 0;
  byte n = 0;

  for (int i = 0; i < samplesLength; i++)
  {
    delayTime = samples[i];
    n = 1;
    if (delayTime < 0)
    {
      delayTime = -delayTime;
      n = 0;
    }

    digitalWrite(PIN_GDO0, n);
    delayMicroseconds(delayTime);

    if (i % 100 == 0)
    {
      Serial.print("."); // progress indicator
      yield();           // feed watchdog
    }
  }

  digitalWrite(PIN_GDO0, 0);
  Serial.println("\n[transmit] ✅ Transmission completed");
  delay(20);
}