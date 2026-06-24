#include <arduino.h>
#include "Shade.h"


CShade::CShade()  // default constructor
{
  pin[0] = 0;
  pin[1] = 0;
  durationDown = 0;
  reinitTime = 0;

  pos = 0;
  reqPos = 0;
  act = _IDLE_;

  lastRunTime = 0;
}
    
CShade::CShade(uint8_t _pinA, uint8_t _pinB, uint8_t _durationDown)
  :CShade()  // call default constructor first
{
  pin[0] = _pinA;
  pin[1] = _pinB;
  durationDown = _durationDown;
}
    
CShade::~CShade()  // destructor
{
}

void CShade::RequestPosition(float _req)
{
  if(_req<0)  // request way UP... will never be reached, keep on moving _UP_
    reqPos = -999.0;
  else if(_req>100)  // request way DOWN... keep moving _DOWN_
    reqPos =  999.0;
  else
    reqPos = _req;

  reinitTime = 0.0;
}

void CShade::ReinitPosition()
{
  reinitTime = 25/100.0 /*25% default move*/ * (float)durationDown/1.03 + 2.0 /*2sec calibration*/;
}
    
uint8_t CShade::RunLoop()
{
  uint8_t ret;
  unsigned long currentTime;

  currentTime = millis();

  if(act==_IDLE_ && pos<reqPos-1.0)  // move downward
  {
    act = MoveDown();
    reinitTime = 0.0;  // no reinit when going down
  }
  else if(act==_DOWN_ && pos<reqPos)  // keep moving down
  {
    pos+=100.0/(float)durationDown * 0.001*(float)((unsigned long)(currentTime-lastRunTime)) /*dt*/;  // 100% / time_FullMove * dt
    if (pos>100) pos = 100.0;
  }
  
  else if(act==_IDLE_ && pos>reqPos+1.0) // move upward
  {
    act = MoveUp();

    if(reqPos==0.0)
      reinitTime = 2.0;  // keep moving up, another 2sec to "calibrate" to position 0
    else
      reinitTime = 0.0;
  }
  else if(act==_UP_ && pos>reqPos)  // keep moving up
  {
    pos-=100.0/((float)durationDown*1.03 /*Scale Dur Up/Down*/) * 0.001*(float)((unsigned long)(currentTime-lastRunTime)) /*dt*/;
    if (pos<0) pos = 0.0;
  }

  else if(act==_IDLE_ && reinitTime>0) // move upward to "calibrate": ReinitPosition()
  {
    act = MoveUp();   
  }
  else if(act==_UP_ && reinitTime>0)  // keep moving up to "calibrate"
  {
    reinitTime -= 0.001*(float)((unsigned long)(currentTime-lastRunTime)) /*dt*/;
  }
  
  else if(act==_UP_ || act==_DOWN_) // requested position achieved - stop actuation, return notification
  {
    act = Stop();
  }
  else
  {
    act = _IDLE_;  // coming from _STOP_...
  }

  lastRunTime = currentTime;
  return(act);
}


uint8_t CShade::MoveDown()
{
    digitalWrite(pin[0], HIGH);
    pinMode(pin[0], OUTPUT);
    digitalWrite(pin[1], LOW);
    pinMode(pin[1], OUTPUT);
    return(_DOWN_);
}

uint8_t CShade::MoveUp()
{
    digitalWrite(pin[0], LOW);
    pinMode(pin[0], OUTPUT);
    digitalWrite(pin[1], HIGH);
    pinMode(pin[1], OUTPUT);
    return(_UP_);
}

uint8_t CShade::Stop()
{
  //digitalWrite(pin[0], LOW);
  pinMode(pin[0], INPUT);
  //digitalWrite(pin[1], LOW);
  pinMode(pin[1], INPUT);  

  return(_STOP_);
}


float CShade::GetPosition()
{
  return(pos);
}

uint8_t CShade::GetActivity()
{
  return(act);
}

uint8_t CShade::CancelActivity()
{
  act = Stop();

  reqPos = pos;  // cmd where we are... so nothing left to do!
  reinitTime = 0;  // cancel reinit

  act = _IDLE_;  // reset to "waiting"
  return(act);
}
