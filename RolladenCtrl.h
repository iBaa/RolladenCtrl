
#define STATE_NONE     0
#define STATE_IDLE    10
#define STATE_MOVING  20
#define STATE_REINIT  30
//extern uint8_t state;

#define UISTATE_STD   40
#define UISTATE_CFG   50
//extern uint8_t uiState;

#define SHADE_CNT   7
typedef struct {
  uint8_t pin[2];
  uint8_t durationDown100;  // [sec] duration of full down move
} SHADECONFIG_T;


extern class CShade *pShade[SHADE_CNT];
//extern uint8_t activeShade;

extern boolean DCF_DaylightSavingsTime;  // used in OpAuto to determine sunrise/sunset time
#include <Encoder.h>
extern Encoder myEnc;

extern boolean buttonUp_raw, buttonUp_pressed, buttonUp_lastPressed;
extern boolean buttonDown_raw, buttonDown_pressed, buttonDown_lastPressed;
extern boolean buttonUp_hold, buttonUp_lastHold;
extern boolean buttonDown_hold, buttonDown_lastHold;

extern boolean encSw_raw, encSw_pressed, encSw_lastPressed;
extern boolean encSw_hold, encSw_lastHold;

extern int32_t enc_cnt, enc_lastCnt;



inline int32_t modulo(int32_t x,int32_t N) {
  return (x % N + N) %N;
}