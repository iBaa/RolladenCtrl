
#define geographischeBreite_grad  ( 50)  // Mainz - 50° nördlicher Breite
#define geographischeLaenge_grad  (- 8)  // Mainz - 8° östlicher Länge

#define OPAUTO_PRGMCNT  4

typedef struct {
  uint8_t hour, minute;
  uint8_t shadesSelected;
  uint8_t position;
  bool    enable, fix, sunrise;
  int8_t  minuteOffset;
} OPAUTOCONFIG_T;
extern OPAUTOCONFIG_T opAutoConfig[OPAUTO_PRGMCNT];


class COpAuto {

  public:
    COpAuto();
    ~COpAuto();

    void    EvalInputs(void);
    uint8_t Run(uint8_t state);  // return update state
    void    UpdatePrgmTiming(OPAUTOCONFIG_T opAutoConfig_T[]);

  private:
    uint8_t day_lastSunTiming;  // last day sunrise/-set was determined
    uint8_t minute_lastCheck;  // last "minute" OpAuto activity was checked

    uint32_t enc_cntSaved;  // store Cnt for sub-inputs - shade position, time

  public:
    uint8_t activePrgm;  // used in Display...
    uint8_t activeUiElem;  // rolling through uiElems, shade position, time
    boolean UiElem_Lvl2;  // position, time - shift Display/EncCnt to this single Elem
};

int32_t TimeToEncCnt(OPAUTOCONFIG_T *prgm);
void EncCntToTime(int32_t time, OPAUTOCONFIG_T *prgm);
