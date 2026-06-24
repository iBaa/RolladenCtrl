#include <arduino.h>

#include "RolladenCtrl.h"
#include "OpAuto.h"
#include "Shade.h"

#include "TimeLib.h"

#include "Dprintf.h"


OPAUTOCONFIG_T opAutoConfig[OPAUTO_PRGMCNT] =  // predefined... store in EEPROM
{
// time   shades      pos en fix sun  min
  { 7,00, 0b01111111,  0,  1,  0,  1, 10},  // open around/after sunrise
  {20,00, 0b01111111, 90,  1,  0,  0, 20},  // close around/after sunset
  { 9,00, 0b00001111, 60,  0,  1,  0,  0},  // eg summer -> 9:00, close Erker 60%
  {16,30, 0b00001111,  0,  0,  1,  0,  0},
};


uint16_t dayOfYear(uint8_t day, uint8_t mon)
{
  return( (mon-1)*365/12 + day);
}

void getSunTiming(float *sunrise, float *sunset)
{
  // Sonnenauf-/untergang
  // https://de.wikipedia.org/wiki/Lichter_Tag
  uint16_t n;  // day of year
  n = dayOfYear(day(), month());
  //n = dayOfYear(5,7);  // debug

  float deklination_rad;
  deklination_rad = -23.44 * 2*PI/360 * cos(2*PI/360 * (n+10)); 
  
  float halberLichterTag_hour;
  halberLichterTag_hour = acos(-tan(geographischeBreite_grad * (2*PI/360))*tan(deklination_rad)) / (2*PI/24);

  // https://de.wikipedia.org/wiki/Zeitzone
  float Mittag;
  Mittag = 12.0 +
           (15+geographischeLaenge_grad) / 15.0 +  // "local noon" - MEZ/MESZ origin at 15° East - earth rotation (24h)->360° - 1h->15° slides
           (DCF_DaylightSavingsTime?1.0:0.0);  // modify for Summertime/daylight savings time (+1 hour)
  *sunrise = min(max(Mittag - halberLichterTag_hour, 0), 24);  
  *sunset = min(max(Mittag + halberLichterTag_hour, 0), 24);

  Dprintln2("mittag:", Mittag);
  Dprintln2("halberLichterTag:", halberLichterTag_hour);
  Dprintln2("sunrise:", *sunrise);
  Dprintln2("sunset:", *sunset);
}


COpAuto::COpAuto()  // default constructor
{
  day_lastSunTiming = 0;
  minute_lastCheck = 0;

  activePrgm = 0;
  activeUiElem = 0;
  UiElem_Lvl2 = 0;
}

COpAuto::~COpAuto()  // destructor
{
}


void COpAuto::UpdatePrgmTiming(OPAUTOCONFIG_T opAutoConfig_T[])
{
  float sunrise, sunset;
  getSunTiming(&sunrise, &sunset);

  int p;
  for(p=0; p<OPAUTO_PRGMCNT; p++)
  {
    if( opAutoConfig[p].fix )  // fix time - keep time setting
    {
      //opAutoConfig[p].hour
      //opAutoConfig[p].minute
    }
    else  // relative to sunrise/-set
    {
      float prgmTime;
      prgmTime = (opAutoConfig[p].sunrise?sunrise:sunset) + opAutoConfig[p].minuteOffset/60.0;
      prgmTime = min(max(prgmTime,0), 24);
      opAutoConfig[p].hour = floor(prgmTime);
      opAutoConfig[p].minute = 60*(prgmTime-floor(prgmTime));
    }

    //Dprintln2("opAuto_hour:", opAuto[p].hour);
    //Dprintln2("opAuto_minute:", opAuto[p].minute);
  }
}


uint8_t COpAuto::Run(uint8_t state)
{
  boolean act=false;

  if( day()!=day_lastSunTiming )  // new day - about 0:00 - determine sunrise/sunset
  {
    day_lastSunTiming = day();
    
    UpdatePrgmTiming(opAutoConfig);
  }


  if( minute()!=minute_lastCheck )  // automatic shade control - new "minute", check todo...
  {
    minute_lastCheck = minute();

    if( state==STATE_IDLE )  // no active task?
    {
      int p;
      for(p=0; p<OPAUTO_PRGMCNT; p++)  // check all opAutoConfigs and request enabled
      {
        if( opAutoConfig[p].enable &&
            hour()==opAutoConfig[p].hour && minute()==opAutoConfig[p].minute )  // automatic shade control, do prgm/config #p
        {
          int i;
          for(i=0; i<SHADE_CNT; i++)
          {
            if(bitRead(opAutoConfig[p].shadesSelected, i))
            {
              pShade[i]->RequestPosition(opAutoConfig[p].position);
            }
          }
          act = true;  // signal scheduled activity
        }
      }
    }  // STATE_IDLE
  }  // minuteLastCheck

  return(act);
}


void COpAuto::EvalInputs(void)
{
  if(activeUiElem==8 /*position*/ && UiElem_Lvl2)
  {
    opAutoConfig[activePrgm].position = modulo( (enc_cnt>>2)*5, 105 );
  }
  else if(activeUiElem==10 /*time*/ && UiElem_Lvl2)
  {
    EncCntToTime(modulo( enc_cnt>>2, 170 ), &opAutoConfig[activePrgm]);
  }
  else
  {
    activeUiElem = modulo( enc_cnt>>2 /*divide by 4*/, 11 /* ??? */);  // Encoder value -> select config fields to modify
  }

  if( encSw_pressed && !encSw_lastPressed && !encSw_hold )
  {
    Dprintln2("UIElem:", activeUiElem);

    if(activeUiElem==0)  // roll through configs/prgm
    {
      activePrgm++;
      if( activePrgm >= OPAUTO_PRGMCNT)
      {
        activePrgm = 0;
      }
    }
    else if(activeUiElem>=1 && activeUiElem<=7)  // enable/disable single shades
    {
      uint8_t i = activeUiElem-1;
      bitWrite( opAutoConfig[activePrgm].shadesSelected, i, !bitRead(opAutoConfig[activePrgm].shadesSelected, i) );  // invert enable
    }
    else if(activeUiElem==8)  // position
    {
      UiElem_Lvl2 = !UiElem_Lvl2;
      if(UiElem_Lvl2)  // modify shade position request
      {
        enc_cntSaved = enc_cnt;
        myEnc.write((opAutoConfig[activePrgm].position/5)<<2);
      }
      else  // back to prgm
      {
        myEnc.write(enc_cntSaved);
      }
    }
    else if(activeUiElem==9)  // enable
    {
      opAutoConfig[activePrgm].enable = !opAutoConfig[activePrgm].enable;
    }
    else if(activeUiElem==10)  // time
    {
      UiElem_Lvl2 = !UiElem_Lvl2;
      if(UiElem_Lvl2)  // modify time - sunrise, sunset, fix
      {
        enc_cntSaved = enc_cnt;
        myEnc.write(TimeToEncCnt(&opAutoConfig[activePrgm])<<2);
      }
      else  // back to prgm
      {
        myEnc.write(enc_cntSaved);
      }
    }
  }
}


/*
  10min/step
  fix time:       00:00..23:50  -> 6*24=144 steps
  around sunrise: -60..:00..+60 -> 13 steps
  around sunset:  -60..:00..+60 -> 13 steps
  total: 170
*/
int32_t TimeToEncCnt(OPAUTOCONFIG_T *prgm)
{
  uint32_t cnt;

  if(prgm->fix)  // 00:00..23:50
  {
    cnt = (prgm->hour*60 + prgm->minute)/10;
  }
  else  // -60..:00..60 - ignore hour
  {
    // around sunrise - :00 is at step 144+7
    // around sunset - :00 is at step 144+13+7

    cnt = 144 + (prgm->sunrise?0:13) + 6;
    cnt = cnt + prgm->minuteOffset/10;  // "before" subtract from :00
  }
  return(cnt);
}

void EncCntToTime(int32_t cnt, OPAUTOCONFIG_T *prgm)
{
  cnt = modulo(cnt, 170);

  if(cnt>=0 && cnt<144)  // fix, 00:00..23:50
  {
    prgm->fix = true;
    prgm->sunrise = false;
    prgm->hour = cnt*10/60;
    prgm->minute = modulo(cnt*10, 60);
    prgm->minuteOffset = 0;
  }
  else if(cnt>=144 && cnt<144+13)  // around sunrise
  {
    prgm->fix = false;
    prgm->sunrise = true;
    prgm->hour = 0;
    prgm->minute = 0;
    prgm->minuteOffset = (cnt-144-6)*10;
  }
  else if(cnt>=144+13 && cnt<144+13+13)  // around sunset
  {
    prgm->fix = false;
    prgm->sunrise = false;
    prgm->hour = 0;
    prgm->minute = 0;
    prgm->minuteOffset = (cnt-144-13-6)*10;
  }
}