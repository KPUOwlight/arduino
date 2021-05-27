#include "MyWifiControl.h"
#include "LedDisplay.h"
#include "PowerControl.h"
#include "LedBattery.h"   //베터리 핀 및 관련 LED 핀 정의// 
#include "PirControl.h"



//-----------------------------------------------------------------------------------------------------//
//LedBattery.h에 정의 됨
//#define _DEBUG_  0
//#define _NOUSE_WIFI_  1
//-----------------------------------------------------------------------------------------------------//

#define ARDUINO_ARCH_ESP32 1

//온도측정//
#define ThermistorPin 27 

//
CMyWifiControl  mywifi;
CPowerControl   powerControl;
CPirControl     PirControl;
int nSwitchValue = 1;

int nTestFlag = 0;

//-----------------------------------------------------------------------------------------------------//




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);// 시리얼포트 통신 초기와
  Serial.println("START");


  pinMode(0, INPUT);// 스위치
  nSwitchValue = digitalRead(0);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED1_COM, OUTPUT);
  pinMode(LED2_COM, OUTPUT);

  pinMode(BAT_CHARGE1, OUTPUT);
  pinMode(LED_LIGHT1, OUTPUT);
  pinMode(BAT_CHARGE2, OUTPUT);
  pinMode(LED_LIGHT2, OUTPUT);

  pinMode(BAT_1, INPUT);// 핀모드 선언
  pinMode(BAT_2, INPUT);// 핀모드 선언
  pinMode(SOLAR, INPUT);// 핀모드 선언
  pinMode(ThermistorPin, INPUT);// 핀모드 선언
  
  //---------------------------------------------------------------------------------------------------//
  //PIR, PWM 관련//
  pinMode(PIR_SENSOR, INPUT); 
  #if defined(ARDUINO_ARCH_ESP32)
    ledcSetup(0, 490, 8);
    ledcAttachPin(PWM_PIN, 0);

  #else
    pinMode(21, OUTPUT);
  #endif   
  //---------------------------------------------------------------------------------------------------//

  CLEAR_LED();
  //---------------------------------------------------------------------------------------------------//
  //---------------------------------------------------------------------------------------------------//
  
  powerControl.StartUp(&PirControl);
  PirControl.StartUp();

  mywifi.SetPirControl(&PirControl);
  
  //---------------------------------------------------------------------------------------------------//
  //---------------------------------------------------------------------------------------------------//
  nTestFlag = 0;
}




void loop() {
  
  
  // put your main code here, to run repeatedly:
  delay(1000);



#if 0
   
  //테스트를 위하여//
  if(Serial.available() > 0){
    String temp = Serial.readStringUntil('\n');
    Serial.printf("\n\nread serial : %s\n\n", temp.c_str());
    
    if (temp == "0")
    {
      //mywifi.ConnectForUpdate();
      nTestFlag = 0;
      
    }
    else if (temp == "1")
    {
       nTestFlag = 1;
       /*
       int nSwitch  = digitalRead(0);// 스위치 핀 값을 읽음
       Serial.print("Switch :");
       Serial.println(nSwitch);
       */
       //ESP.restart();
    }
    else if (temp == "2")
    {
      nTestFlag = 2;
    }
    else if (temp == "3")
    {
      
      mywifi.ConnectToComWifi();
    }
    //-------------------------------------------------------------//
    //-------------------------------------------------------------//
    else if (temp == "0 0 0")
    {
      CLEAR_LED();
    }
    else if (temp == "1 1 1")
    {
      LED_ON(LED1, LED1_COM);
    }
    else if (temp == "0 1 1")
    {
      LED_OFF(LED1, LED1_COM);
    }
    else if (temp == "1 1 2")
    {
      LED_ON(LED1, LED2_COM);
    }
    else if (temp == "0 1 2")
    {
      LED_OFF(LED1, LED2_COM);
    }
    
    else if (temp == "1 2 1")
    {
      LED_ON(LED2, LED1_COM);
    }
    else if (temp == "0 2 1")
    {
      LED_OFF(LED2, LED1_COM);
    }
    else if (temp == "1 2 2")
    {
      LED_ON(LED2, LED2_COM);
    }
    else if (temp == "0 2 2")
    {
      LED_OFF(LED2, LED2_COM);
    }
    
    else if (temp == "1 3 1")
    {
      LED_ON(LED3, LED1_COM);
    }
    else if (temp == "0 3 1")
    {
      LED_OFF(LED3, LED1_COM);
    }
    else if (temp == "1 3 2")
    {
      LED_ON(LED3, LED2_COM);
    }
    else if (temp == "0 3 2")
    {
      LED_OFF(LED3, LED2_COM);
    }

    else if (temp == "1 4 1")
    {
      LED_ON(LED4, LED1_COM);
    }
    else if (temp == "0 4 1")
    {
      LED_OFF(LED4, LED1_COM);
    }
    else if (temp == "1 4 2")
    {
      LED_ON(LED4, LED2_COM);
    }
    else if (temp == "0 4 2")
    {
      LED_OFF(LED4, LED2_COM);
    }
  
  }
#endif
  //--------------------------------------------------------------------------------------------//
  //프로그램 재시잘 스위치 정의//
  int nSwitchTemp = digitalRead(0);
  if( nSwitchValue == 0 && nSwitchTemp == 1)
  {
      Serial.println("!! Restart switch On");
      ESP.restart();
  }
  nSwitchValue = nSwitchTemp;
  //--------------------------------------------------------------------------------------------//
  //베터리 충전//
  //--------------------------------------------------------------------------------------------//
  if(nTestFlag)
  {
  
    powerControl.Charging(nTestFlag);  
    return;
  }
  //--------------------------------------------------------------------------------------------//
  //--------------------------------------------------------------------------------------------//

  
  powerControl.Charging();
  
  //LED 표시//
  LED_OFF(LED1, LED1_COM);
  LED_OFF(LED2, LED1_COM);
  LED_OFF(LED3, LED1_COM);

  if( powerControl.m_fChargingVoltage)
  {
    if( powerControl.m_fChargingVoltage > 1.0)  LED_ON(LED1, LED1_COM);
    if( powerControl.m_fChargingVoltage > 2.0)  LED_ON(LED2, LED1_COM);
    if( powerControl.m_fChargingVoltage > 3.0)  LED_ON(LED3, LED1_COM);
    
  }
  else
  {
    if( powerControl.m_fUsefulVoltage > 1.0)  LED_ON(LED1, LED1_COM);
    if( powerControl.m_fUsefulVoltage > 2.0)  LED_ON(LED2, LED1_COM);
    if( powerControl.m_fUsefulVoltage > 3.0)  LED_ON(LED3, LED1_COM);
  }
  //--------------------------------------------------------------------------------------------//

  
  //온도 측정//
  float fEsp32Temp = Esp32Temperature();
  float fInnerTemp = InnerTemperature();

  
  //--------------------------------------------------------------------------------------------//
  #if _NOUSE_WIFI_

  int nLedOnHour = PirControl.GetLED_OnHour();
  int nLedOffHour= PirControl.GetLED_OffHour();
  
  //CMyWifiControl ( mywifi )에 서버 전송을 위한 자료저장
  mywifi.SetSendInfo(powerControl.m_solar_vin, powerControl.m_bat1_vin, powerControl.m_bat2_vin,  fEsp32Temp, fInnerTemp,nLedOnHour,nLedOffHour);

  //무선 연결 및 통신//
  //if( powerControl.m_fUsefulVoltage > 3.0f){
  if( powerControl.m_solar_vin > 3.7f){  
    mywifi.StartUp(); 
    mywifi.Loop();
    
  }
  

  #endif
  
  //--------------------------------------------------------------------------------------------//
  //PIR 센서에 따른 PWM 컨트롤, CPirControl::Control() 함수 내에서 정의된 시간에 작동을 한다.
  PirControl.Control(powerControl.m_fUsefulVoltage);    
  
  //
  //--------------------------------------------------------------------------------------------//
  LED_ON(LED4, LED1_COM);// 아래 하나는 항상 켜둔다//
  //--------------------------------------------------------------------------------------------//
     
}
