/*
 * Rolladen-Control

  Digital Outputs - DOUT
  Wohnen - 5
    a) weiss/blau - 8/9
    b) gelb/braun - 11/12
       weiss/braun - 8/12
       weiss/gelb - 8/11
       weiss/blau - 8/10
  Küche - 2
    c) gelb/braun - 9/10
       weiss/blau - 9/11

  Digital Inputs - DIN
  Buttons  
    UP - 6
    DOWN - 7
  
  Encoder
    A - 3 (interrupt)
    B - 4
    Switch - 5 (or ANALOG?)

  DCF - Funkuhr
    DIN2 (interrupt)  
*/

/*
needs Libraries:
- Encoder (PaulStoffregen) - rotational switch
- U8x8/U8g2 (olikraus@gmail.com) - display
- DCF77 (ThijsElenbaas) - radio controller clock - get "latest", not v1.0.0 - https://github.com/thijse/Arduino-DCF77
- TimeLib (Michael Margolis)
*/

#include <avr/wdt.h>
#include "RolladenCtrl.h"

#include "Shade.h"
#include "Display.h"
#include "OpAuto.h"
#include "OpManual.h"

// Enable the Dprintf() functions. Attention: This should be 0 in the release version to save memory (vfprintf uses 2368 bytes)
//#define ENABLE_DPRINTF 1
#include "Dprintf.h"  // global #define ENABLE_DPRINTF in "Dprintf.h" 

#include "TimeLib.h"
#include "DCF77.h"

#define DCF_PIN 2  // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0  // Interrupt number associated with pin
DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT, false /* flank - high/low active */);
boolean DCF_DaylightSavingsTime = 0;  // used in OpAuto to determine sunrise/sunset time

//#define ENCODER_OPTIMIZE_INTERRUPTS   // This optional setting causes Encoder to use more optimized code. Unfortunately it doesn't work with the DCC library
#include <Encoder.h>                  // Rotary Encoder library from Paul Stoffregen (https://www.pjrc.com/teensy/td_libs_Encoder.html)
#define ENCODER_A_PIN   3             // Best Performance: both pins have interrupt capability
#define ENCODER_B_PIN   4             // Good Performance: only the first pin has interrupt capability
#define ENCODER_SW_PIN  5             // Encoder switch pin (Other side connected to GND)
Encoder myEnc(ENCODER_A_PIN, ENCODER_B_PIN);


#define BUTTON_UP   6  // UP at DIN 6
#define BUTTON_DOWN 7  // DOWN at DIN 7
#define BUTTON_DEBOUNCE_CNT 5  // debounce count in 10ms slices

COpAuto OpAuto;

static const SHADECONFIG_T shadeConfig[SHADE_CNT] = {
  { 8,  9, 15},  // down 16s, up 18s
  {11, 12, 17}, { 8, 12, 17}, { 8, 11, 24}, { 8, 10, 15},
  { 9, 10, 15}, { 9, 11, 24}   // down ?, up ?
};
/*
  Wohnen - 5
    a) weiss/blau - 8/9
    b) gelb/braun - 11/12
       weiss/braun - 8/12
       weiss/gelb - 8/11
       weiss/blau - 8/10
  Küche - 2
    c) gelb/braun - 9/10
       weiss/blau - 9/11
*/

/*
  STATE_IDLE    10
  STATE_MOVING  20
  STATE_REINIT  30
*/
uint8_t state = STATE_IDLE;

/*
  UISTATE_STD   40
  UISTATE_CFG   50
*/
uint8_t uiState = UISTATE_STD;

class CShade *pShade[SHADE_CNT];
uint8_t activeShade = 255;
uint8_t lastActiveShade = 255;  // debug only


class CDisplay *pDisplay;

unsigned long timeHeartbeat, timeDCF77, timeDCF77Signal;
unsigned long timeButton;
unsigned long timeMain, timeLastTask, timeStatistics;
unsigned long timeUIActive;

boolean DCF77Signal, lastDCF77Signal = false;

unsigned long turnaround_last = 0, turnaround_max = 0;

boolean buttonUp_raw = false, buttonUp_debounced = false;
int8_t buttonUp_cnt = 0;
boolean buttonUp_pressed = false, buttonUp_lastPressed = false;
boolean buttonUp_hold = false, buttonUp_lastHold = false;
int8_t buttonUp_holdCnt = 0;

boolean buttonDown_raw = false, buttonDown_debounced = false;
int8_t buttonDown_cnt = 0;
boolean buttonDown_pressed = false, buttonDown_lastPressed = false;
boolean buttonDown_hold = false, buttonDown_lastHold = false;
int8_t buttonDown_holdCnt = 0;

boolean encSw_raw = false, encSw_debounced = false;
int8_t encSw_cnt = 0;
boolean encSw_pressed = false, encSw_lastPressed = false;
boolean encSw_hold = false, encSw_lastHold = false;
int8_t encSw_holdCnt = 0;

int32_t enc_cnt = 0, enc_lastCnt = 0;


void setup() {
  // put your setup code here, to run once:
  uint8_t i;

  // DEBUG
  #if ENABLE_DPRINTF
  // Initiate Serial communication.
    Serial.begin(9600);
  #endif
  Dprintf("RollandenCtrl - setup()\n");

  // Heartbeat - built-in LED
  timeHeartbeat = millis();
  pinMode(LED_BUILTIN, OUTPUT);

  // setup shade config
  for (i=0; i<SHADE_CNT; i++)  // init shades
  {
    pinMode(shadeConfig[i].pin[0], INPUT);  // Up - high impedance
    pinMode(shadeConfig[i].pin[1], INPUT);  // Down - high impedance

    pShade[i] = new CShade(shadeConfig[i].pin[0],shadeConfig[i].pin[1], shadeConfig[i].durationDown100);  // create pShade objects
  }

  // DCF77 clock
  pinMode(DCF_PIN, INPUT);
  digitalWrite(DCF_PIN, HIGH);  // internal pull-up
  DCF.Start();

  // setup CDisplay object
  pDisplay = new CDisplay();

  // Buttons and Switches
  pinMode(BUTTON_UP, INPUT);
  digitalWrite(BUTTON_UP, HIGH);  // internal pull-up
  pinMode(BUTTON_DOWN, INPUT);
  digitalWrite(BUTTON_DOWN, HIGH);  // internal pull-up

  pinMode(ENCODER_SW_PIN, INPUT);
  digitalWrite(ENCODER_SW_PIN, HIGH);  // internal pull-up

  // timer defaults
  timeMain = 0;
  timeLastTask = 0;
  timeStatistics = 0;
  timeButton = 0;
  timeUIActive = 0;
  timeDCF77 = 0;
  timeDCF77Signal = 0;

  // watchdog
  wdt_enable(WDTO_4S);  // just in case!
}


void loop() {
  // watchdog
  wdt_reset();

  // Statistics - turnaround-time
  turnaround_max = max( turnaround_max, millis()-turnaround_last );
  turnaround_last = millis();

  if ((millis()-timeStatistics) > 10000)  // update every 10sec
  {
    timeStatistics = millis();

    Dprintf("turnaround_max (ms): %i\n", turnaround_max);
    turnaround_max = 0;
  }

  // Heartbeat
  // DCF77 - duplicate signal to builtin Heartbeat LED
  DCF77Signal = !digitalRead(DCF_PIN);  // invert - high/low active
  if(DCF77Signal != lastDCF77Signal)
  {
    lastDCF77Signal = DCF77Signal;
    digitalWrite(LED_BUILTIN, DCF77Signal);
    timeDCF77Signal = millis();
  }

  // DCF77timeout - fall back to Heartbeat
  if ((millis()-timeDCF77Signal) > 5000)
  {
    DCF77Signal = digitalRead(LED_BUILTIN);  // ...forward to CDisplay::displayTime()
    if ((millis()-timeHeartbeat) > 1000)
    {
      timeHeartbeat = millis();
      digitalWrite(LED_BUILTIN, !DCF77Signal);  // invert and reflect on internal LED
    }  // timeHeartbeat -> 1s 
  }  // timeDCF77Signal -> 5s

  // Mainloop - debounce inputs
  if ((millis()-timeButton) > 10)  // User Interface
  {
    timeButton = millis();

    // up/down buttons
    buttonUp_raw = !digitalRead(BUTTON_UP);
    debounce(buttonUp_raw, &buttonUp_debounced, &buttonUp_cnt, BUTTON_DEBOUNCE_CNT); // 10ms, 5steps    
    delayOn(buttonUp_debounced, &buttonUp_hold, &buttonUp_holdCnt, 50);  // 10ms, 50steps
    buttonUp_pressed = buttonUp_debounced && !buttonUp_hold; 

    buttonDown_raw = !digitalRead(BUTTON_DOWN);
    debounce(buttonDown_raw, &buttonDown_debounced, &buttonDown_cnt, BUTTON_DEBOUNCE_CNT);
    delayOn(buttonDown_debounced, &buttonDown_hold, &buttonDown_holdCnt, 50);
    buttonDown_pressed = buttonDown_debounced && !buttonDown_hold; 

    encSw_raw = !digitalRead(ENCODER_SW_PIN);
    debounce(encSw_raw, &encSw_debounced, &encSw_cnt, BUTTON_DEBOUNCE_CNT);
    delayOn(encSw_debounced, &encSw_hold, &encSw_holdCnt, 50);
    encSw_pressed = encSw_debounced && !encSw_hold;

    // push/rotate control
    enc_cnt = myEnc.read();

    if (buttonUp_raw ||
        buttonDown_raw ||
        encSw_raw ||
        enc_cnt!=enc_lastCnt)
    {
      timeUIActive = millis();
    }

    enc_lastCnt = enc_cnt;
  }  // timeButton -> 10ms

  // reset UI
  if (timeUIActive!=0 &&
      (millis()-timeUIActive) > 15000)  // reset UI after 15sec of no-op
  {
    myEnc.write(0);  // write back... reset rotational switch pos

    timeUIActive = 0;
    uiState = UISTATE_STD;  // default to std screen

    OpAuto.activePrgm = 0;  // reset to Prgm #0, UiElem 0
    OpAuto.activeUiElem = 0;

    pDisplay->clear();
  }

  // check DCF77 radio clock
  if((millis()-timeDCF77) > 1000)
  {
    timeDCF77 = millis();
    
    time_t DCFTime = DCF.getTime();  // Check if new DCF77 time is available
    if( DCFTime!=0 &&  // new DCF time reading
        abs(now()-DCFTime)>10 )  // difference to internal time>10sec - power up init or major adjust
    {
      DCF_DaylightSavingsTime = DCF.getSummerTime();
      setTime(DCFTime);
      OpAuto.UpdatePrgmTiming(opAutoConfig);  // force opAuto timing re-evaluation

      Dprintf("time is updated\n");
    }
    /*
    char str[8];
    snprintf(str, 8, "%2u:%02u", hour(), minute());
    Dprintln2("time: ", str);
    */
  }  // timeDCF77 -> 1s
  
  // MainLoop UserInterface
  if ((millis()-timeMain) > 100)  // UserInterface
  {
    timeMain = millis();
    
    // DEBUG
    if(!buttonUp_lastPressed && buttonUp_pressed)
      Dprintf("ButtonUP\n");
    if(!buttonUp_lastHold && buttonUp_hold)
      Dprintf("ButtonUPHOLD\n");
    if(buttonUp_lastHold && !buttonUp_hold)
      Dprintf("ButtonUPHOLDRELEASE\n");
    if(!buttonDown_lastPressed && buttonDown_pressed)
      Dprintf("ButtonDOWN\n");
    if(!buttonDown_lastHold && buttonDown_hold)
      Dprintf("ButtonDOWNHOLD\n");
    if(buttonDown_lastHold && !buttonDown_hold)
      Dprintf("ButtonDOWNHOLDRELEASE\n");
    if(!encSw_lastPressed && encSw_pressed)
      Dprintf("EncoderSwitch\n");
    if(!encSw_lastHold && encSw_hold)
      Dprintf("EncoderSwitchHOLD\n");


    if( !encSw_lastHold && encSw_hold )  // switch uiState
    {
      if(uiState==UISTATE_STD)
      {
        uiState = UISTATE_CFG;
      }
      else
      {
        uiState = UISTATE_STD;
      }

      myEnc.write(0);  // write back... reset rotational switch pos
      pDisplay->clear();
    }

    switch(uiState)
    {
      case UISTATE_CFG:
        OpAuto.EvalInputs();
        pDisplay->displayProgram(OpAuto.activePrgm, OpAuto.activeUiElem, OpAuto.UiElem_Lvl2);
        break;
      case UISTATE_STD:
      default:
        uint8_t act;
        act = OpManual_EvalInputs(state, activeShade);
        if( act==STATE_MOVING || act==STATE_REINIT )  // updated state if activity scheduled
        {
          activeShade = 0;
          state = act;
          Dprintf("MANUAL MOVE\n");
        }

        uint8_t bf_heartbeat = (activeShade<8)?(1<<activeShade):0;
        pDisplay->displayWindows(shadesSelected[OpManual_shadeConfig], bf_heartbeat, 0b00000000, 0b01111111 /*show all shades*/);
        pDisplay->displayState(state, buttonUp_pressed, buttonDown_pressed);
        pDisplay->displayTime(DCF77Signal);
        break;
    }

    // check for automatic operation request
    if( OpAuto.Run(state) )  // true if activity scheduled
    {
      activeShade = 0;
      state=STATE_MOVING;
      Dprintf("AUTO MOVE\n");
    }

    // last button state
    buttonUp_lastPressed = buttonUp_pressed;
    buttonUp_lastHold = buttonUp_hold;

    buttonDown_lastPressed = buttonDown_pressed;
    buttonDown_lastHold = buttonDown_hold;

    encSw_lastPressed = encSw_pressed;
    encSw_lastHold = encSw_hold;
  }  // timeMain -> 100ms


  if((millis()-timeLastTask) > 100)  // shade activity supervision
  {    
    timeLastTask = millis();
  
    if (lastActiveShade != activeShade)  // debug only
    {
      Dprintln2("activeShade:", activeShade)
      lastActiveShade = activeShade;
    }

    while(activeShade<SHADE_CNT)  // active movement - run pShade-RunLoop, if done move next shade
    {
      uint8_t act;      
      act = pShade[activeShade]->RunLoop();  // move shade
      if (act==_IDLE_)
      {
        activeShade++;  // next shade
        if(activeShade>=SHADE_CNT)
        {
          activeShade=255;  // no more activity
          state = STATE_IDLE;
        }
      }
      else
      {
        break;  // go on next main loop
      }
    }  // activeShade
  }  // timeLastTask -> 100ms
}



void debounce(boolean in, boolean *out, int8_t *cnt, int8_t maxCnt)
{
  if (in)  // pressed
  {
    if( *cnt>=maxCnt )
    {
      *out = true;
    }
    else
      ++(*cnt);
  }
  else  // released
  {
    if( *cnt<=0 )
    {
      *out = false;
    }
    else
      --(*cnt);
  }
}

void delayOn(boolean in, boolean *out, int8_t *cnt, int8_t maxCnt)
{
  if (in)  // pressed
  {
    if( *cnt>=maxCnt )
    {
      *out = true;
    }
    else
      ++(*cnt);
  }
  else  // released - direct off
  {
    *cnt = 0;
    *out = false;
  }
}

