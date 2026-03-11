#ifndef Shade_h
#define Shade_h

#include <arduino.h>

#define _IDLE_ 0
#define _STOP_ 1
#define _DOWN_ 2
#define _UP_ 3

class CShade {
  private:
    uint8_t pin[2]={0,0}, durationDown=0;
    uint8_t act;
    float   pos, reqPos, reinitTime;

    unsigned long lastRunTime;

  public:
    CShade();
    CShade(uint8_t _pinA, uint8_t _pinB, uint8_t _durationDown);
    ~CShade();
 
    uint8_t RequestPosition(float _req);  // pos: 0..100
    uint8_t ReinitPosition();
    float   GetPosition();
    uint8_t GetActivity();
    uint8_t CancelActivity();

    uint8_t RunLoop();

    uint8_t MoveUp(); 
    uint8_t MoveDown(); 
    uint8_t Stop();
};

#endif
