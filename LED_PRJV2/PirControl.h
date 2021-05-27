#ifndef _PIR_CONTROL_
#define _PIR_CONTROL_


#include <time.h>

#define PIR_SENSOR      19
#define PWM_PIN         21

//오후 7 - 05시 까지 작동 // 
#define LED_ON_START_HOUR  19 
#define LED_ON_END_HOUR    5
#define LED_ON_INCREMENT   1

//와이파이 안될 때//
#define LED_START_TIMER   3600// 1시간
//



#define LED_ON_DURATION 10 //LED 가 켜져있는 시간(초)//




class CPirControl
{

  //-----------------------------------------------------------------------------------------------//
  //LED 가 켜지고 꺼지는 시간//
  int   m_nLedOnHour;
  int   m_nLedOffHour;
  //-----------------------------------------------------------------------------------------------//

  int    m_nPwmStatus;
     
  int    m_nDiffPwm;  // m_nPwmStatus 에서 차감해 나가기 위한 값 ( 255 / LED_ON_DURATION  ) //
  time_t m_lOnTime;

  //와이파이가 안될 때//
  time_t m_lStartTimer;
  time_t m_lStopTimer;

  void   ledLoop();

  int    nIncrementFlg; 

  //------------------------------------------------------------------------------------------------//
  //------------------------------------------------------------------------------------------------//
  unsigned int m_nPirCount; 

public:
  //------------------------------------------------------------------------------------------------//
  //------------------------------------------------------------------------------------------------//
  
  void  StartUp();
  int   Control(float fVol);

  //------------------------------------------------------------------------------------------------//
  CPirControl();
  ~CPirControl();
  //------------------------------------------------------------------------------------------------//
  void  SetTimerStart(time_t ltime);
  void  SetTimerStop();
  //------------------------------------------------------------------------------------------------//
  int   GetLED_OnHour();
  int   GetLED_OffHour();

  void  SetLedOnOffHour(int nLedOn, int mLedOff);
  //------------------------------------------------------------------------------------------------//
  unsigned int   GetPirCount();
  void  ResetPirCount();
  //------------------------------------------------------------------------------------------------//
  
};



#endif//_PIR_CONTROL_
