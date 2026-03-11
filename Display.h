// Display.h

#ifndef Display_h
#define Display_h

class CDisplay
{
  public:
    CDisplay(void);
    
    void clear(void);

    void displayWindows(uint8_t bf_selected, uint8_t bf_heartbeat, uint8_t bf_flash, uint8_t bf_show);
    void displayTime(bool heartbeat);
    void displayState(uint8_t state, bool buttonUp, bool buttonDown);

    void displayProgram(uint8_t prgm, uint8_t active, uint8_t flash);
    
//    void displayMenu();
};

#endif  // Display_h
