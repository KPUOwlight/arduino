#include <Arduino.h>
#include "LedBattery.h"
#include "PowerControl.h"
#include "LedDisplay.h"
#include <time.h>


//-----------------------------------------------------------------------------------------------------//
CPowerControl::CPowerControl()
{

  m_R1 = 30000.0; //저항 R1(30K) 
  m_R2 = 7500.0;  //저항 R2(7.5k)
  
  
  m_bat1_vout = 0.0;
  m_bat1_vin  = 0.0;
  m_bat1_value = 0;
  
  m_bat2_vout = 0.0;
  m_bat2_vin  = 0.0;
  m_bat2_value = 0;
  
  
  m_solar_vout = 0.0;
  m_solar_vin  = 0.0;
  m_solar_value = 0;

  //Vin = Vout * ((m_R1+m_R2)/m_R2)
  m_R = (m_R1+m_R2)/m_R2;
  
  m_pPirControl = NULL;
  m_fUsefulVoltage = 0.0;
  m_fChargingVoltage = 0.0;
  //
  //
}

CPowerControl::~CPowerControl()
{


  
}
void 
CPowerControl::init()
{
  digitalWrite(BAT_CHARGE1,LOW);
  digitalWrite(LED_LIGHT1, LOW);

  digitalWrite(BAT_CHARGE2,LOW);
  digitalWrite(LED_LIGHT2, LOW);

  m_fUsefulVoltage = 0.0;
  m_fChargingVoltage = 0.0;
}

int   
CPowerControl::chargingBat1()
{

  digitalWrite(BAT_CHARGE2,LOW);
  digitalWrite(LED_LIGHT2, HIGH);
  m_fUsefulVoltage = m_bat1_vin;
  m_fChargingVoltage = m_bat1_vin;

  if(m_bat1_vin > MAX_CHARGED_VOLT1 || m_bat1_value >= 4096)
  {
#if _DEBUG_
    Serial.println("BAT1 MAX!! return");
#endif    
    return 0;
  }
  
  //1 번 밧데리 충전
  digitalWrite(BAT_CHARGE1,HIGH);
  digitalWrite(LED_LIGHT1, LOW);
  
#if _DEBUG_
  Serial.printf(" ----- BAT1 Charging !!  USE Voltage(%4.2f) \n",m_fUsefulVoltage);
#endif  
  return 11;
}


int   
CPowerControl::chargingBat2()
{

  digitalWrite(BAT_CHARGE1,LOW);
  digitalWrite(LED_LIGHT1, HIGH);
  m_fUsefulVoltage = m_bat2_vin;
  m_fChargingVoltage = m_bat2_vin;
  
  if( m_bat2_vin > MAX_CHARGED_VOLT2 || m_bat2_value >= 4096)
  {
#if _DEBUG_
    Serial.println("BAT2 MAX!! return");
#endif    
    return 0;
  }

  //2 번 밧데리 충전
  digitalWrite(BAT_CHARGE2,HIGH);
  digitalWrite(LED_LIGHT2, LOW);
#if _DEBUG_
  Serial.printf(" ----- BAT2 Charging !!  USE Voltage(%4.2f)  \n",m_fUsefulVoltage);
#endif  

  return 22;
}



void  
CPowerControl::setUseBat()
{
  
#ifdef USE_ONLY_BAT1
  setUseOnlyBat1();
  return;
#endif//USE_ONLY_BAT1  

#ifdef USE_ONLY_BAT2
  setUseOnlyBat2();
  return;
#endif//USE_ONLY_BAT2  

  
  digitalWrite(BAT_CHARGE1,LOW);
  digitalWrite(BAT_CHARGE2,LOW);
  m_fChargingVoltage = 0.0;
      
  if( m_bat1_vin > 3.0f && m_bat2_vin > 3.0f)
  {
      if( m_bat1_vin < m_bat2_vin + ADDITIONAL_VOLTAGE)
      {
        digitalWrite(LED_LIGHT1, LOW);
        digitalWrite(LED_LIGHT2, HIGH);
        m_fUsefulVoltage = m_bat2_vin;

#if _DEBUG_
        Serial.printf(" ----- use bat2 (m_bat1_vin < m_bat2_vin + ADDITIONAL_VOLTAGE) !!  Voltage(%4.2f)   \n",m_fUsefulVoltage);
#endif      
      }
      else
      {
        digitalWrite(LED_LIGHT1, HIGH);
        digitalWrite(LED_LIGHT2, LOW);
        m_fUsefulVoltage = m_bat1_vin;
#if _DEBUG_
        Serial.printf(" ----- use bat1 (m_bat1_vin > m_bat2_vin + ADDITIONAL_VOLTAGE) !!  Voltage(%4.2f)   \n",m_fUsefulVoltage);
#endif      
      }
  }
  else
  {

      if( m_bat1_vin < m_bat2_vin)
      {
        digitalWrite(LED_LIGHT1, LOW);
        digitalWrite(LED_LIGHT2, HIGH);
         m_fUsefulVoltage = m_bat2_vin;

#if _DEBUG_
         Serial.printf(" ----- use bat2 (m_bat1_vin < m_bat2_vin) !!  Voltage(%4.2f)   \n",m_fUsefulVoltage);
#endif      
      }
      else
      {
        digitalWrite(LED_LIGHT1, HIGH);
        digitalWrite(LED_LIGHT2, LOW);
        m_fUsefulVoltage = m_bat1_vin;
#if _DEBUG_        
        Serial.printf(" ----- use bat1 (m_bat1_vin > m_bat2_vin) !!  Voltage(%4.2f)   \n",m_fUsefulVoltage);
#endif      
      }

  
  }

}

void  
CPowerControl::setUseOnlyBat1()
{
  digitalWrite(BAT_CHARGE1,LOW);
  digitalWrite(BAT_CHARGE2,LOW);

  digitalWrite(LED_LIGHT1, HIGH);
  digitalWrite(LED_LIGHT2, LOW);

  m_fChargingVoltage = 0.0;
  m_fUsefulVoltage = m_bat1_vin;

#if _DEBUG_
  Serial.printf(" ----- use bat1 only  !!  Voltage(%4.2f)   \n",m_fUsefulVoltage);
#endif
}

void  
CPowerControl::setUseOnlyBat2()
{
  digitalWrite(BAT_CHARGE1,LOW);
  digitalWrite(BAT_CHARGE2,LOW);

  digitalWrite(LED_LIGHT1, LOW);
  digitalWrite(LED_LIGHT2, HIGH);

  m_fChargingVoltage = 0.0;
  m_fUsefulVoltage = m_bat2_vin;

#if _DEBUG_
  Serial.printf(" ----- use bat2 only  !!  Voltage(%4.2f)   \n",m_fUsefulVoltage);
#endif
}

//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//

int  
CPowerControl::bat1FirstCharging()
{
#if _DEBUG_  
  Serial.println("--- BAT1 FIRST CHARGING --- ");
#endif  
  if( m_bat1_vin <  MAX_CHARGED_VOLT1)
  {
      if( m_solar_vin >  m_bat1_vin)
      {
        return chargingBat1();
      }
  }

  if( m_solar_vin > m_bat2_vin)
  {
      return chargingBat2();
  }
#if _DEBUG_  
  Serial.println("SOLAR <=  BAT  !!");
#endif  
  return -201;

}


int  
CPowerControl::batExchgGharging()
{

#if _DEBUG_  
  Serial.println("--- BAT EXCHANGED CHARGING --- ");
#endif
  
  if( m_bat1_vin > 3.0f && m_bat2_vin > 3.0f)
  {
      if( m_bat1_vin < m_bat2_vin + ADDITIONAL_VOLTAGE)
      {
        if( m_solar_vin <= m_bat1_vin)
        {
          
#if _DEBUG_
          Serial.println("SOLAR <=  BAT1!!");
#endif          
          return -2;
        }
        
        return chargingBat1();
         
      }
      else
      {
        if( m_solar_vin <= m_bat2_vin)
        {
#if _DEBUG_
          Serial.println("SOLAR <=  BAT2!!");
#endif          
          return -3;
        }
        
        return chargingBat2();
    
        
      }
  }
  else
  {

      if( m_bat1_vin < m_bat2_vin)
      {
        if( m_solar_vin <= m_bat1_vin)
        {
#if _DEBUG_
          Serial.println("SOLAR <=  BAT1!!");
#endif          
          return -2;
        }
        
        return chargingBat1();
         
      }
      else
      {
        if( m_solar_vin <= m_bat2_vin)
        {
#if _DEBUG_          
          Serial.println("SOLAR <=  BAT2!!");
#endif          
          return -3;
        }
        
        return chargingBat2();
    
        
      }

  
  }
  return 1;


  
}

int   
CPowerControl::bat1OnlyCharging()
{
#if _DEBUG_
  Serial.println("--- BAT1 ONLY CHARGING --- ");
#endif
  m_fUsefulVoltage  = m_bat1_vin;
  m_fChargingVoltage= m_bat1_vin;

  
  if( m_bat1_vin <  MAX_CHARGED_VOLT1)
  {
      if( m_solar_vin >  m_bat1_vin)
      {
          digitalWrite(BAT_CHARGE2,LOW);
          digitalWrite(LED_LIGHT2, LOW);
  
          //1 번 밧데리 충전
          digitalWrite(BAT_CHARGE1,HIGH);
          digitalWrite(LED_LIGHT1, LOW);
          
#if _DEBUG_
          Serial.printf(" ----- BAT1 Only Charging !!  USE Voltage(%4.2f) \n",m_fUsefulVoltage);
#endif          
          return 301;

      }
      else
      {
#if _DEBUG_
          Serial.println("SOLAR <=  BAT1  !!");
#endif      
      }
  }
  else
  {
#if _DEBUG_     
     Serial.printf("BAT MAX :%4.2f  !!\n", m_bat1_vin);
#endif  
  }
  
  return -301;
  
}


int   
CPowerControl::bat2OnlyCharging()
{

  Serial.println("--- BAT2 ONLY CHARGING --- ");

  m_fUsefulVoltage  = m_bat2_vin;
  m_fChargingVoltage= m_bat2_vin;

  
  if( m_bat2_vin <  MAX_CHARGED_VOLT2)
  {
      if( m_solar_vin >  m_bat2_vin)
      {
          digitalWrite(BAT_CHARGE1,LOW);
          digitalWrite(LED_LIGHT1, LOW);
  
          //2 번 밧데리 충전
          digitalWrite(BAT_CHARGE2,HIGH);
          digitalWrite(LED_LIGHT2, LOW);
          
#if _DEBUG_
          Serial.printf(" ----- BAT2 Only Charging !!  USE Voltage(%4.2f) \n",m_fUsefulVoltage);
#endif          
          return 401;

      }
      else
      {
#if _DEBUG_      
          Serial.println("SOLAR <=  BAT2  !!");
#endif      
      }
  }
  else
  {
#if _DEBUG_     
     Serial.printf("BAT MAX :%4.2f  !!\n", m_bat2_vin);
#endif  
  }
  
  return -401;
  
}

//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//


void 
CPowerControl::StartUp(CPirControl* pPirControl)
{
  
  m_pPirControl = pPirControl;
  init();

}



int  
CPowerControl::Charging(int nTestFlag)
{
  
  m_bat1_value  = analogRead(BAT_1);// 34번핀 값을 읽음
  m_bat2_value  = analogRead(BAT_2);// 35번핀 값을 읽음
  m_solar_value = analogRead(SOLAR);// 32번핀 값을 읽음

  
  m_solar_vout = (m_solar_value * BASE_VOLTAGE) / 4096.0; //12비트 분해능이므로 4096으로 나눠줌
  m_solar_vin  = m_solar_vout * m_R; //저항분배법칙 으로 계산

  m_bat1_vout = (m_bat1_value * BASE_VOLTAGE) / 4096.0; //12비트 분해능이므로 4096으로 나눠줌
  m_bat1_vin  = m_bat1_vout * m_R; //저항분배법칙 으로 계산

  m_bat2_vout = (m_bat2_value * BASE_VOLTAGE) / 4096.0; //12비트 분해능이므로 4096으로 나눠줌
  m_bat2_vin  = m_bat2_vout * m_R; //저항분배법칙 으로 계산

#if _DEBUG_

  Serial.printf("read battery : %d  %d (solar :%d)\n", m_bat1_value,m_bat2_value,m_solar_value);
  
  Serial.print(m_solar_vin); //입력되는 전압값 출력
  Serial.println(" SOLAR volt");

  Serial.print(m_bat1_vin); //입력되는 전압값 출력
  Serial.println(" BAT1 volt");

  Serial.print(m_bat2_vin); //입력되는 전압값 출력
  Serial.println(" BAT2 volt");

  if(m_bat1_vin > MAX_CHARGED_VOLT1 || m_bat1_value >= 4096)
  {
    Serial.println("BAT1 MAX!!");
  }
  if( m_bat2_vin > MAX_CHARGED_VOLT2 || m_bat2_value >= 4096)
  {
    Serial.println("BAT2 MAX!!");
  }


#endif


  //----------------------------------------------------------------------------------------//
  //----------------------------------------------------------------------------------------//
  time_t ltime;
  time( &ltime);   

  int nWifiOff = 0;
  //
  struct tm* pTime = localtime(&ltime);
  if (pTime->tm_year +1900 < 2019) {
    LED_ON(LED1, LED2_COM);
    //와이파이 접속이 안될 수도 있다.//
    nWifiOff = 1;
  }
  //
  if( nWifiOff == 0)
  {
    if( pTime->tm_hour >  POWER_CHARGE_HOUR )
    {
      // POWER_CHARGE_HOUR 이후는 충정을 하지 않는다.
      setUseBat();
#if _DEBUG_      
      Serial.println(" POWER CHARGING TIMEOVER");
#endif      
      return 0;
    }
    
    if (m_solar_value <= 0) 
    {
#if _DEBUG_      
      Serial.println("SOLAR MIN!!");
#endif      
      setUseBat();
      
      
      return -1 ;
    }
    
  }
  else // 와이파이와 접속되지 않음
  {
    if(m_solar_vin < 0.7f) 
    {
      
#if _DEBUG_
      Serial.println("SOLAR MIN!! -- WIFI OFF");
#endif      
      
      setUseBat();
      //와이파이가 안될 때는 타이머를 사용한다.//
      if(m_pPirControl) m_pPirControl->SetTimerStart(ltime);
      
      return -1 ;
    }
    else
    {
      if(m_pPirControl) m_pPirControl->SetTimerStop();
    }
    
  }
  //----------------------------------------------------------------------------------------//
  //----------------------------------------------------------------------------------------//

  if(nTestFlag == 1)
  {
    return chargingBat1();
  }
  else if(nTestFlag == 2)
  {
    return chargingBat2();
  }
  //----------------------------------------------------------------------------------------//
  //----------------------------------------------------------------------------------------//
  #ifdef USE_ONLY_BAT1 // 배터리 하나만 사영//

  bat1OnlyCharging();

  #else
  #ifdef USE_ONLY_BAT2 // 배터리 하나만 사영//
  bat2OnlyCharging();
  #else
  
  #ifdef  BAT1_FIRST_CHARGING
  return bat1FirstCharging();  
  #else
  return batExchgGharging();  
  #endif

  #endif//USE_ONLY_BAT2
  #endif//USE_ONLY_BAT1
  
  //----------------------------------------------------------------------------------------//
  //----------------------------------------------------------------------------------------//
  return 1;
}


//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//
