#ifndef _MYWIFICONTROL_
#define _MYWIFICONTROL_


#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <time.h>
#include <WiFiUdp.h>

#include "LedDisplay.h"
#include "PirControl.h"



const int MY_WIFI_VER = 100;

class CMyWifiControl
{

  //-----------------------------------------------------------------------------------------------//
  //LED 가 켜지고 꺼지는 시간//
  int   m_nLedOnHour;
  int   m_nLedOffHour;

  //
  //서버 접속 정보
  String    m_strUpdateSSID;
  String    m_strUpdatePWD;
  String    m_strCommSSID;
  String    m_strCommPWD;
  int       m_nUpdateConnTime;
  int       m_nCommConnTime;
  //
  //-----------------------------------------------------------------------------------------------//


  time_t 	  m_lConTime;
  int       m_nResetDuration;

	int			  m_nWifiConnect;
  WiFiUDP   m_udp;

  //서버 전송을 위한 자료저장//
  float     m_fSolarVin;  
  float     m_fBatVin1;
  float     m_fBatVin2;
  float     m_fEsp32Temp;
  float     m_fInnerTemp;
  int       m_nSedSeq ;

  

  int       m_nStarted;
  

	void			init_connection();
  void      send_info();
  void      received_info();

  CPirControl* m_pPirControl;
  void      updating_server();
  
public:
  

	bool ConnectForUpdate();
  bool ConnectToComWifi();

  //아두이노가 시작할때 실행하도록 한다.
  //2 분간 업데이트용 와이파이로 접속후//
  //통신용 와이파이에 접속한다.
  void StartUp();
	void Loop();

	void SetSendInfo(float fSolar, float fBat1, float fBat2, float fEsp32Temp, float fInnerTemp, int nLedOnHour, int nLedOffHour);
	void SetPirControl(CPirControl* pPirControl);
	
	
	CMyWifiControl();
	~CMyWifiControl();

};

#endif//_MYWIFICONTROL_
