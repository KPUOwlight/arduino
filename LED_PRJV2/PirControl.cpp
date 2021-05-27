#include <Arduino.h>
#include "PirControl.h"
#include "LedBattery.h"
//-----------------------------------------------------------------------------------------------------//
CPirControl::CPirControl()
{
  m_nPwmStatus = -1;
  m_nDiffPwm = (int) ( 255 / LED_ON_DURATION );  // m_nPwmStatus 에서 차감해 나가기 위한 값 ( 255 / LED_ON_DURATION  ) //
  m_lOnTime  = 0;
  m_lStartTimer = 0;
  m_lStopTimer  = 0;
  nIncrementFlg = 0;

  m_nLedOnHour = LED_ON_START_HOUR;
  m_nLedOffHour= LED_ON_END_HOUR;


  m_nPirCount = 0;
  
}


CPirControl::~CPirControl()
{


  
}

void 
CPirControl::ledLoop()
{
#ifdef LED_ON_INCREMENT

  if( m_nPwmStatus == 0)
  {
    m_nPwmStatus = 255;
    nIncrementFlg= 50;
    
    time(&m_lOnTime);  
    Serial.println("SET PWM LED ON" );      
  }
  else if( m_nPwmStatus >  0)
  {
    time_t ltime;
    time(&ltime);    
    int nDiff = (int)difftime(ltime,m_lOnTime);
    

    if( nDiff > 1 )
    {
      m_lOnTime = ltime;
      m_nPwmStatus -= m_nDiffPwm * nDiff;
      nIncrementFlg +=  m_nDiffPwm * nDiff;     
    }
    
    Serial.printf("PWM : %d  (Diff:%d) \n" ,m_nPwmStatus, nDiff);
  }

  if( m_nPwmStatus >  0)
  {
    if( nIncrementFlg >  255) nIncrementFlg = 255;
    ledcWrite(0, nIncrementFlg);
         
  }
  else
  {
    m_nPwmStatus = -1;
    nIncrementFlg = 0;
    ledcWrite(0, 0);   
  }


#else
  if( m_nPwmStatus == 0)
  {
    m_nPwmStatus = 255;
    time(&m_lOnTime);  
    Serial.println("SET PWM LED ON" );      
  }
  else if( m_nPwmStatus >  0)
  {
    time_t ltime;
    time(&ltime);    
    int nDiff = (int)difftime(ltime,m_lOnTime);
    

    if( nDiff > 1 )
    {
      m_lOnTime = ltime;
      m_nPwmStatus -= m_nDiffPwm * nDiff;
      
    }
    
    Serial.printf("PWM : %d  (Diff:%d) \n" ,m_nPwmStatus, nDiff);
  }

  if( m_nPwmStatus >  0)
  {
    ledcWrite(0, m_nPwmStatus);     
  }
  else
  {
    m_nPwmStatus = -1;
    ledcWrite(0, 0);   
  }
#endif
}

//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//
void  
CPirControl::StartUp()
{
  
}


int   
CPirControl::Control(float fVol)
{
  //-------------------------------------------------------------------------//
  //-------------------------------------------------------------------------//
  if( fVol < 2.7)
  {
      if( m_nPwmStatus <= 0)
      {
         return -101;
      }
    
  }
  //-------------------------------------------------------------------------//
  //-------------------------------------------------------------------------//

  int val= digitalRead(PIR_SENSOR);
  
  time_t ltime;
  time(&ltime);
  struct tm* pTime = localtime(&ltime);

  //-------------------------------------------------------------------------//
  int nWifiOff = 0;
  if (pTime->tm_year +1900 < 2019)
  {
     nWifiOff = 1;
  }
  //-------------------------------------------------------------------------//
  if(nWifiOff == 0)
  {
    if( pTime->tm_hour >= m_nLedOnHour || pTime->tm_hour <= m_nLedOffHour) 
    {
      
      if ( val == HIGH ) {
         
         m_nPwmStatus = 0; //m_nPwmStatus == 0 이면 ledLoop 안에서 LED를 ON 한다//
         m_nPirCount++;
      }
  
    }
    Serial.printf("PIR  time:%02d \n", pTime->tm_hour );
  
  }
  else
  {
    int nDiff = 0;
    if( m_lStartTimer > 0)
    {
      
      nDiff = difftime(ltime, m_lStartTimer);
      if( nDiff > LED_START_TIMER)
      {
        if ( val == HIGH )
        {
          m_nPwmStatus = 0; //m_nPwmStatus == 0 이면 ledLoop 안에서 LED를 ON 한다//
          m_nPirCount++;
        }
      }

      
    
    }
    Serial.printf("PIR  timer:%u  diff:%d\n", m_lStartTimer, nDiff);
    
  }
  //-------------------------------------------------------------------------//
  
  Serial.printf("PIR_SENSOR: %d  \n" ,val);
  ledLoop();
  return 1;
}

//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//

void  
CPirControl::SetTimerStart(time_t ltime)
{
  if( m_lStartTimer <= 0)
  {
      m_lStartTimer = ltime;

      //
      struct tm* pTime = localtime(&ltime);
      char szDate[100];
      sprintf(szDate,"%04d-%02d-%02d %02d:%02d:%02d",
          pTime->tm_year +1900,pTime->tm_mon + 1,pTime->tm_mday,
          pTime->tm_hour,pTime->tm_min,pTime->tm_sec);

      Serial.printf("SET TIMER : %s\n" ,szDate );
      //
      
  }
  
  m_lStopTimer = 0;
  
  
}

void  
CPirControl::SetTimerStop()
{
  
  int nDiff = 0;
  //
  time_t ltime;
  time(&ltime);

  if( m_lStopTimer <= 0)
  {
     m_lStopTimer = ltime;
  }
  else
  {
    nDiff = difftime(ltime, m_lStopTimer);
    if( nDiff > 300)
    {
      m_lStartTimer = 0;
    }
    
  }

  
  struct tm* pTime = localtime(&ltime);
  char szDate[100];
  sprintf(szDate,"%04d-%02d-%02d %02d:%02d:%02d",
      pTime->tm_year +1900,pTime->tm_mon + 1,pTime->tm_mday,
      pTime->tm_hour,pTime->tm_min,pTime->tm_sec);

  Serial.printf("STOP TIMER : %s (Stop diff :%d)\n" ,szDate,nDiff );
  //

}
//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//
int   
CPirControl::GetLED_OnHour()
{
  return m_nLedOnHour;
}
int   
CPirControl::GetLED_OffHour()
{
  return m_nLedOffHour;
}

void  
CPirControl::SetLedOnOffHour(int nLedOn, int mLedOff)
{
  m_nLedOnHour = nLedOn;
  m_nLedOffHour= mLedOff;  
}

unsigned int   
CPirControl::GetPirCount()
{
  return m_nPirCount;
}

void  
CPirControl::ResetPirCount()
{
  m_nPirCount = 0;

}

  
//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//
