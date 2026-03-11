#include <arduino.h>

#include "RolladenCtrl.h"
#include "OpManual.h"
#include "Shade.h"

#include "Dprintf.h"

int8_t OpManual_shadeConfig=0;



uint8_t OpManual_EvalInputs(uint8_t state, uint8_t activeShade)
{
  int i;
  uint8_t act = STATE_NONE;

  OpManual_shadeConfig = modulo( enc_cnt>>2 /*divide by 4*/, SHADESSELECTED_CNT);  // limit to 0..SHADESSELECTED_CNT-1

  if( (buttonUp_pressed && !buttonDown_pressed && buttonDown_raw) || (!buttonUp_pressed && buttonUp_raw && buttonDown_pressed) )  // something fishy - both pressed, one still unconfirmed
  {
    Dprintf("UP-DOWN - one still unconfirmed\n");
  }
  else if( buttonUp_pressed && buttonDown_pressed && (!buttonUp_lastPressed || !buttonDown_lastPressed) )  // both pressed - extra function?
  {
    Dprintf("UP-DOWN\n");
  }
  else if( buttonUp_hold && buttonDown_hold && (!buttonUp_lastHold || !buttonDown_lastHold) )  // both press and hold - reinit
  {
    for(i=0; i<SHADE_CNT; i++)
    {
      pShade[i]->ReinitPosition();
    }
    act = STATE_REINIT;  // signal scheduled activity
    Dprintf("UP-DOWN HOLD - REINIT\n");
  }  // buttonUp_hold && buttonDown_hold

  else if(!buttonDown_lastPressed && buttonDown_pressed)  // pressed: down
  {
    if( state==STATE_IDLE )  // start moving down, push task into first active pShade() object
    {
      for(i=0; i<SHADE_CNT; i++)  // find active shade
      {
        if(bitRead(shadesSelected[OpManual_shadeConfig], i))
        {
          break;
        }
      }
      if(i>=0 && i<SHADE_CNT)
      {
        pShade[i]->RequestPosition(999.0);  // go wayyy down...
        act = STATE_MOVING;  // signal scheduled activity
      }
      Dprintf("DOWN\n");
    }
    else if( state==STATE_MOVING )  // cancel down
    {
      for(i=0; i<SHADE_CNT; i++)
      {
        pShade[i]->CancelActivity();
      }
      Dprintf("CANCEL DOWN\n");
    }
  }  // buttonDown_pressed

  else if(!buttonUp_lastPressed && buttonUp_pressed)  // pressed: up
  {
    if( state==STATE_IDLE )  // start moving up
    {
      for(i=0; i<SHADE_CNT; i++)  // find active shade
      {
        if(bitRead(shadesSelected[OpManual_shadeConfig], i))
        {
          break;
        }
      }
      if(i>=0 && i<SHADE_CNT)
      {
        pShade[i]->RequestPosition(-999.0);  // go wayyy up...
        act = STATE_MOVING;  // signal scheduled activity
      }
      Dprintf("UP\n");
    }
    else if( state==STATE_MOVING )  // cancel up
    {
      for(i=0; i<SHADE_CNT; i++)
      {
        pShade[i]->CancelActivity();
      }
      Dprintf("CANCEL UP\n");
    }
  }  // buttonUp_pressed

  else if( !buttonDown_lastHold && buttonDown_hold )  // hold: down - keep going down
  {
    Dprintf("UP HOLD\n");
  }  // buttonDown_hold
  else if( !buttonUp_lastHold && buttonUp_hold )  // hold: up - keep going up
  {
    Dprintf("DOWN HOLD\n");
  }  // buttonUp_hold

  else if( (buttonDown_lastHold && !buttonDown_hold) || (buttonUp_lastHold && !buttonUp_hold) )  // up/down Hold released - set shade req to current setting
  {
    if( state==STATE_MOVING )
    {
      float pos;
      pos = pShade[activeShade]->GetPosition();
      for(i=0; i<SHADE_CNT; i++)
      {
        if(bitRead(shadesSelected[OpManual_shadeConfig], i))
        {
          pShade[i]->RequestPosition(pos);
        }
      }
      Dprintf("RELEASE HOLD - SET CURRENT POSITION\n");
    }
  }  // up/down hold released

  else if( buttonDown_lastPressed && !buttonDown_pressed)  // down released - set shade req to 90 (about closed) - don't, if switching to "hold"
  {
    if( state==STATE_MOVING )
    {
      for(i=0; i<SHADE_CNT; i++)
      {
        if(bitRead(shadesSelected[OpManual_shadeConfig], i))
        {
          pShade[i]->RequestPosition(90.0);  // about closed
        }
      }
      Dprintf("RELEASE DOWN - SET 90%\n");
    }
  }  // down released
  else if(buttonUp_lastPressed && !buttonUp_pressed)  // up released - set shade to 0 (open) - don't, if switching to "hold"
  {
    if( state==STATE_MOVING )
    {
      for(i=0; i<SHADE_CNT; i++)
      {
        if(bitRead(shadesSelected[OpManual_shadeConfig], i))
        {
          pShade[i]->RequestPosition(0.0);  // completely open
        }
      }
      Dprintf("RELEASE UP - SET 0%\n");
    }
  }  // up released
  
  return(act);
}
