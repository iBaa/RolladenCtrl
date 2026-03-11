
#include <arduino.h>

#include "Dprintf.h"

#include <stdarg.h>
#include <WString.h>

//------------------------------------------------------
void Dprintf_proc(const __FlashStringHelper *format, ...)
//------------------------------------------------------
{
  char buf[50];
  va_list ap;
  va_start(ap, format);
  #ifdef __AVR__
     vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
  #else
     vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
  #endif

  va_end(ap);
  Serial.print(buf);
}
