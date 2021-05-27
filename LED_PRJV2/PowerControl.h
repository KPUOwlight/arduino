#ifndef _POWER_CONTROL_
#define _POWER_CONTROL_

#define  BAT_1  34  //bat1 충전전압 측정
#define  BAT_2  35  //bat2 충전전압 측정
#define  SOLAR  32  //태양광 충전전압 측정

#define BASE_VOLTAGE      4.3 //Vin을 계산할 때 사용한다.

#define MAX_CHARGED_VOLT1 4.2 //BAT1 이 완충되었을 때
#define MAX_CHARGED_VOLT2 3.3 //BAT2 이 완충되었을 때 

#define POWER_CHARGE_HOUR  19  // POWER_CHARGE_HOUR 이후는 충정을 하지 않는다.

#define ADDITIONAL_VOLTAGE 0.9  //BAT1 이 BAT2 보다 크다.//

#define BAT1_FIRST_CHARGING 1   //베터리1 먼저 충전


//#define USE_ONLY_BAT1       1   //배터리 하나만 사용//


#include "PirControl.h"


class CPowerControl
{

  float m_R1;  //저항 R1(30K) 
  float m_R2;  //저항 R2(7.5k)
  float m_R;   //(m_R1+m_R2)/m_R2
  
  float m_bat1_vout;
  int   m_bat1_value;
  
  float m_bat2_vout;
  int   m_bat2_value;
  
  
  float m_solar_vout;
  int   m_solar_value;

  void  init();
  int   chargingBat1();
  int   chargingBat2();
  void  setUseBat();

        
  CPirControl* m_pPirControl;

  //-----------------------------------------------------------------------//
  //-----------------------------------------------------------------------//
  //BAT1 을 먼저 충전//
  int   bat1FirstCharging();
  int   batExchgGharging();
  //
  //배터리 하나만 사용//
  int   bat1OnlyCharging();
  void  setUseOnlyBat1();
  int   bat2OnlyCharging();
  void  setUseOnlyBat2();
  //-----------------------------------------------------------------------//
  //-----------------------------------------------------------------------//
  
public:
  float m_solar_vin;
  float m_bat1_vin;
  float m_bat2_vin;
  
  float m_fUsefulVoltage;
  float m_fChargingVoltage;
  
  void StartUp(CPirControl* pPirControl);
  //--------------------------------------------------------------------------//
  int  Charging(int nTestFlag = 0);
  //--------------------------------------------------------------------------//
  CPowerControl();
  ~CPowerControl();
  //--------------------------------------------------------------------------//

};

#endif//_POWER_CONTROL_
