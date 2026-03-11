

#define SHADESSELECTED_CNT  12

static const uint8_t shadesSelected[SHADESSELECTED_CNT] = {
  0b01111111,  // all shades
  0b01100000,  // Küche
  0b00100000, 0b01000000,  // single shades - Küche
  0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000,  // single shades - Wohnen
  0b00000110,
  0b00011110,
  0b00011111,  // Wohnen
};
// use bitRead(num,pos)

extern int8_t OpManual_shadeConfig;

uint8_t OpManual_EvalInputs(uint8_t state, uint8_t activeShade);  // return updated staet if activity scheduled
