#include "MyWifiControl.h"
#include "MsgParser.h"


//------------------------------------------------------------------------------------------------------//
//매 시간 정각에 접속하는 호스트 -- 업로드용 ( 핸드폰 와이파이)//
const char* update_ssid = "nautes";
const char* update_password = "nautes123";

//UDP Send 용 공용 와이파이 ( 통신사 와이파이)
#if 1
const char* com_ssid       = "Public WIFI@Guro";
const char* com_password   = "";

//-----------------------------------------------------------------------------------------------------//
//정보를 보내기 위한 서버
const char* UDP_SERVER_IP = "125.141.139.9";
const int   UDP_PORT = 16069; 
//-----------------------------------------------------------------------------------------------------//

#else
const char * com_ssid = "KOKI_LAN";
const char * com_password = "KITAEKITAE";

//-----------------------------------------------------------------------------------------------------//
//정보를 보내기 위한 서버
const char* UDP_SERVER_IP = "125.141.139.9";
//const char* UDP_SERVER_IP = "192.168.2.51";
const int   UDP_PORT = 16069; 

//-----------------------------------------------------------------------------------------------------//

#endif


//시간 설정
const char* ntpServer = "pool.ntp.org";

uint8_t timeZone = 9;
uint8_t summerTime = 0; // 3600

int s_hh = 12;      // 시간 설정 변수 < 0 조건 위해 자료형 int
int s_mm = 59;
uint8_t s_ss = 45;
uint16_t s_yy = 2017;
uint8_t s_MM = 11;
uint8_t s_dd = 19;

time_t now;
time_t prevEpoch;
struct tm * timeinfo;



void set_time() {
  struct tm tm_in;
  tm_in.tm_year = s_yy - 1900;
  tm_in.tm_mon = s_MM - 1;
  tm_in.tm_mday = s_dd;
  tm_in.tm_hour = s_hh;
  tm_in.tm_min = s_mm;
  tm_in.tm_sec = s_ss;
  time_t ts = mktime(&tm_in);
  printf("Setting time: %s", asctime(&tm_in));
  struct timeval now = { .tv_sec = ts };
  settimeofday(&now, NULL);
}

void get_NTP() {
  configTime(3600 * timeZone, 3600 * summerTime, ntpServer);
  timeinfo = localtime(&now);
  while (timeinfo->tm_year +1900 == 1970) {
    Serial.println("Failed to obtain time");
    set_time();
    localtime(&now);
    
    
    
    return;
  }
  
  
}


void printLocalTime() {
  if (time(&now) != prevEpoch) {
    Serial.println(time(&now));  // 현재 UTC 시간 값 출력
    timeinfo = localtime(&now);
    int dd = timeinfo->tm_mday;
    int MM = timeinfo->tm_mon + 1;
    int yy = timeinfo->tm_year +1900;
    int ss = timeinfo->tm_sec;
    int mm = timeinfo->tm_min;
    int hh = timeinfo->tm_hour;
    int week = timeinfo->tm_wday;
#if _DEBUG_   
    Serial.print(week); Serial.print(". ");
    Serial.print(yy); Serial.print(". ");
    Serial.print(MM); Serial.print(". ");
    Serial.print(dd); Serial.print(" ");
    Serial.print(hh); Serial.print(": ");
    Serial.print(mm); Serial.print(": ");
    Serial.println(ss); 
#endif    
    prevEpoch = time(&now);
  }
}

//-----------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//

/*
 * Login page
 */

const char* loginIndex = 
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";
 
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";



//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
WebServer m_server(80);
CMyWifiControl::CMyWifiControl()

{
  Serial.println("MY WIFI CONTROL CREATED !!");
  m_lConTime = 0;
  m_nWifiConnect = 0;
  m_nResetDuration = 60;
  init_connection();

  //서버 전송을 위한 자료저장//
  m_fSolarVin = 0.0;  
  m_fBatVin1  = 0.0;  
  m_fBatVin2  = 0.0;  
  m_fEsp32Temp= 0.0;
  m_fInnerTemp= 0.0;
	//

  m_nStarted  = 0;
  m_nSedSeq = 0;


  m_nLedOnHour = 19;
  m_nLedOffHour= 5;
  m_pPirControl= NULL;

  //
  //서버 접속 정보
  m_strUpdateSSID = update_ssid;
  m_strUpdatePWD  = update_password;
  m_strCommSSID   = com_ssid;
  m_strCommPWD    = com_password;
  m_nUpdateConnTime = 0;
  m_nCommConnTime = 30;
  //
  

  
}



CMyWifiControl::~CMyWifiControl()
{
	
	init_connection();

}

void	
CMyWifiControl::init_connection()
{
	WiFi.disconnect(true);
	WiFi.mode(WIFI_OFF);
	Serial.println("WIFI_OFF"); 
  
  m_nWifiConnect = 0;
  m_lConTime = 0;
  
	delay(2000);
}


void 
CMyWifiControl::updating_server()
{
  /*return index page which is stored in serverIndex */
  m_server.on("/", HTTP_GET, []() {
    m_server.sendHeader("Connection", "close");
    m_server.send(200, "text/html", loginIndex);
  });
  m_server.on("/serverIndex", HTTP_GET, []() {
    m_server.sendHeader("Connection", "close");
    m_server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  m_server.on("/update", HTTP_POST, []() {
    m_server.sendHeader("Connection", "close");
    m_server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = m_server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  
  m_server.begin();
  
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//


bool 
CMyWifiControl::ConnectForUpdate()
{
	
	init_connection();
	
	// Connect to WiFi network
  char szSSID[50], szPwd[50];
  memset(szSSID,0x00,sizeof(szSSID));
  memset(szPwd,0x00,sizeof(szPwd));

  m_strUpdateSSID.toCharArray(szSSID,sizeof(szSSID));
  m_strUpdatePWD.toCharArray(szPwd,sizeof(szPwd));

  
  WiFi.begin(szSSID, szPwd);
  
  Serial.print("trying connrct to ");
  Serial.print(m_strUpdateSSID);
	Serial.println(" for update");
  
	// Wait for connection
	int nCount = 0;
	m_nWifiConnect = 0;
	
	int nLedOn = 0;
  CLEAR_LED();
  
  while (WiFi.status() != WL_CONNECTED) {
    nCount++;
    delay(1000);
    Serial.print("@");
    Serial.print(nCount);
    
    //
    //if( nCount % 2 == 0)
    {
        if( nLedOn > 0)
        {
          LED_OFF(LED3,LED2_COM);
          nLedOn = 0;
        }
        else
        {
          LED_ON(LED3,LED2_COM);
          nLedOn = 1;
        }
      
    }
    //
    
    if( nCount > 30) // 10초  //
  	{
      LED_OFF(LED3,LED2_COM);
      Serial.println("");
  	  Serial.println("timeout connection trying");
  		return false;
  	}
  }
  //
  Serial.println("");
  CLEAR_LED();
  
  LED_ON(LED3,LED2_COM);
  //-----------------------------------------------------------//
  //NTP //
  
  get_NTP();
  delay(2000);
  printLocalTime();
  time(&m_lConTime);
  //-----------------------------------------------------------//
  
  m_nWifiConnect = 1;
  m_nResetDuration = 300;
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print(m_strUpdateSSID);
  Serial.println(" for update");
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  byte mac[6]; 
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  
  
  /*return index page which is stored in serverIndex */
  m_server.on("/", HTTP_GET, []() {
    m_server.sendHeader("Connection", "close");
    m_server.send(200, "text/html", loginIndex);
  });
  m_server.on("/serverIndex", HTTP_GET, []() {
    m_server.sendHeader("Connection", "close");
    m_server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  m_server.on("/update", HTTP_POST, []() {
    m_server.sendHeader("Connection", "close");
    m_server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = m_server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  
  m_server.begin();
  
	return true;	
}



bool 
CMyWifiControl::ConnectToComWifi()
{
  init_connection();

  // Connect to WiFi network
  char szSSID[50], szPwd[50];
  memset(szSSID,0x00,sizeof(szSSID));
  memset(szPwd,0x00,sizeof(szPwd));

  m_strCommSSID.toCharArray(szSSID,sizeof(szSSID));
  m_strCommPWD.toCharArray(szPwd,sizeof(szPwd));

  
  WiFi.begin(szSSID, szPwd);
  Serial.print("trying connrct to ");
  Serial.println(m_strCommSSID);
  
  int nCount = 0;
  int nLedOn = 0;
  CLEAR_LED();
   
  while (WiFi.status() != WL_CONNECTED) {
    nCount++;
    //
    delay(1000);
    Serial.print("@");
    Serial.print(nCount);
    //
    //
    //if( nCount % 2 == 0)
    {
        if( nLedOn > 0)
        {
          LED_OFF(LED3,LED2_COM);
          nLedOn = 0;
        }
        else
        {
          LED_ON(LED3,LED2_COM);
          nLedOn = 1;
        }
      
    }
    //
    if( nCount > 15) // 15초  //
    {
      LED_OFF(LED3,LED2_COM);
      Serial.println("");
      Serial.println("timeout connection trying");
      return false;
    }
    //  
  }
  //
  Serial.println("");
  CLEAR_LED();
  
  LED_ON(LED3,LED2_COM);

  
  Serial.print(" Connected to ");
  Serial.println(m_strCommSSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  //-----------------------------------------------------------//

  get_NTP();
  delay(2000);
  printLocalTime();
  time(&m_lConTime);

  //-----------------------------------------------------------//
  m_udp.begin(WiFi.localIP(),UDP_PORT);
  send_info();

  delay(2000);
  received_info();    
  //-----------------------------------------------------------//
  
  m_nWifiConnect = 2;
  m_nResetDuration = 10;
  return true;
}


void 
CMyWifiControl::StartUp()
{

  if(m_nStarted) return ;
  Serial.println("!!      WIFI START UP" );
  m_nStarted = 1;
  if( ConnectForUpdate() == false)
  {
     Serial.println("!!      Start up failed (ConnectForUpdate)" );
     ConnectToComWifi();
  }
  else
  {
    m_nResetDuration = 120;

    Serial.println("!!      Start up success (ConnectForUpdate)" );
    
    time_t ltime;
    time(&ltime);
    
    struct tm* pInfo = localtime(&ltime);
    if(pInfo->tm_year +1900 == 1970) 
    {
      Serial.println("!!      Start up failed (get NTP)" );
      ConnectToComWifi();
    }
  
  }
  
}


void 
CMyWifiControl::Loop()
{
  time_t ltime;
  time( &ltime);   

  //
  struct tm* pTime = localtime(&ltime);
  if (pTime->tm_year +1900 < 2019) {
    LED_ON(LED1, LED2_COM);
    Serial.println("Wifi NOK");
  }
  else
  {
    LED_OFF(LED1,LED2_COM);
    LED_ON(LED2, LED2_COM);
    int nDiff = (int)difftime(ltime,m_lConTime);
    Serial.printf("Wifi OK : m_nWifiConnect:%d  diff:%d  m_nResetDuration:%d\n",m_nWifiConnect,nDiff,m_nResetDuration);
  }
  //
	
	
	if( m_nWifiConnect > 0 )
	{
    //m_nWifiConnect == 1 update wifi, m_nWifiConnect == 2 common wifi//
    if( m_nWifiConnect == 1)
    {
		    m_server.handleClient();	
    }
    
    
    int nDiff = (int)difftime(ltime,m_lConTime);
    
    if (WiFi.status() != WL_CONNECTED || nDiff > m_nResetDuration) {
      
      LED_OFF(LED3,LED2_COM);
      LED_OFF(LED1,LED2_COM);
      
      printLocalTime();
      Serial.printf("WiFi.status = %d Diff:%d( %u - %u)  \n",WiFi.status(),nDiff,ltime,m_lConTime);
      init_connection();
    }
	
	}
 else{
    
    if( ( pTime->tm_hour >= 12 && pTime->tm_hour < 15 )  && pTime->tm_min == m_nUpdateConnTime &&  pTime->tm_sec == 0)
    {
        ConnectForUpdate();
        m_nResetDuration = 300;
      
    }
    else if( ( pTime->tm_hour >= 12 && pTime->tm_hour < 15 ) && pTime->tm_min == m_nCommConnTime &&  pTime->tm_sec == 0)
    {
        ConnectToComWifi();  
    }
 
 }
  
}



//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
// 서버로 정보를 전송한다. ( ConnectToComWifi 함수 안에서 사용 )
//
void
CMyWifiControl::send_info()
{

  time_t ltime;
  time( &ltime);   
  //
  struct tm* pTime = localtime(&ltime);
  char szDate[100];
  sprintf(szDate,"%04d-%02d-%02d %02d:%02d:%02d",
          pTime->tm_year +1900,pTime->tm_mon + 1,pTime->tm_mday,
          pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
  
  
  IPAddress ip = WiFi.localIP();
  char szIP[30];
  sprintf(szIP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);


  byte mac[6]; 
  WiFi.macAddress(mac);
  char szMAC[30];
  sprintf(szMAC, "%02X:%02X:%02X:%02X:%02X:%02X",mac[5],mac[4], mac[3],mac[2],mac[1],mac[0]);

  //
  unsigned Pircount = 0;
  if(m_pPirControl)
  {
    Pircount = m_pPirControl->GetPirCount();
  }
  //
  
  char szPacket[1024];
  memset(szPacket,0x00,sizeof(szPacket));
  sprintf(szPacket,"|LED_INFO|SEQ=%d|VER=%d|DATE=%s|IP=%s|MAC=%s|SOLAR=%2.2f|BAT1=%2.2f|BAT2=%2.2f|ESP_T=%3.2f|INNER_T=%3.2f|LED_ON=%d|LED_OFF=%d|PIR_COUNT=%u|",
          ++m_nSedSeq,MY_WIFI_VER,szDate,szIP,szMAC,m_fSolarVin,m_fBatVin1,m_fBatVin2,m_fEsp32Temp,m_fInnerTemp,m_nLedOnHour,m_nLedOffHour,Pircount);

  int nPacketLen = strlen(szPacket);
  Serial.printf("UDP_PACKET(%d): %s\n", nPacketLen,szPacket);

  //
  if( nPacketLen > 0)
  {
    uint8_t* buffer = (uint8_t*)szPacket;
  
    m_udp.beginPacket(UDP_SERVER_IP, UDP_PORT);
    m_udp.write(buffer, nPacketLen);
    m_udp.endPacket();
  
    Serial.printf("UDP_SEND TO %s:%d\n", UDP_SERVER_IP,UDP_PORT);
  }
  
  //
  if(m_pPirControl)  m_pPirControl->ResetPirCount();
  //

  
}


void 
CMyWifiControl::received_info()
{
    
    char packetBuffer[1024]; 
    memset(packetBuffer,0x00,sizeof(packetBuffer));
    int packetSize = m_udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remoteIp = m_udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(m_udp.remotePort());
  
      // read the packet into packetBufffer
      int len = m_udp.read(packetBuffer, 1024);
      if (len > 0) {
        packetBuffer[len] = 0;
        
        CMsgParser msgParse(packetBuffer,len);
        String strType = msgParse.GetType();
        Serial.print("GetType:");
        Serial.println(strType);

        if( strType == "CHANGE_ONOFF_TIME")
        {
          String strLedOn = msgParse.GetValue("LED_ON");
          String strLedOff= msgParse.GetValue("LED_OFF");

          int nLedOn = strLedOn.toInt();
          int nLedOff= strLedOff.toInt();
          
          Serial.printf("CHANGE_ONOFF_TIME:  LED_ON:%d  LED_OFF:%d\n", nLedOn,nLedOff);

          if(m_pPirControl)
          {
            m_pPirControl->SetLedOnOffHour(nLedOn, nLedOff);  
          }

        }
        else if(strType == "UPDATE_VERSION")
        {
          updating_server();  
        }
        else if(strType == "UPDATE_CONNECTION")
        {
          String strUpdateSSID = msgParse.GetValue("UPDATE_SSID");
          String strUpdatePWD  = msgParse.GetValue("UPDATE_PWD");
          if( strUpdateSSID.length() > 5)
          {
            m_strUpdateSSID = strUpdateSSID;
            m_strUpdatePWD  = strUpdatePWD;
          }

          String strCommSSID = msgParse.GetValue("COMM_SSID");
          String strCommPWD  = msgParse.GetValue("COMM_PWD");
          if( strCommSSID.length() > 5)
          {
            m_strCommSSID = strCommSSID;
            m_strCommPWD  = strCommPWD;
          }
          Serial.print("UPDATE_CONNECTION:");
          Serial.print("  UpdateSSID:");
          Serial.print(m_strUpdateSSID); 
          Serial.print("  UpdatePWD:");
          Serial.print(m_strUpdatePWD);
          
          Serial.print("  CommSSID:");
          Serial.print(m_strCommSSID); 
          Serial.print("  CommPWD:");
          Serial.println(m_strCommPWD);
        }
        else if( strType == "CHANGE_CONNECT_TIME")
        {
          String strUpdateTime = msgParse.GetValue("UPDATE_TIME");
          String strCommonTime= msgParse.GetValue("COMMON_TIME");

          m_nUpdateConnTime = strUpdateTime.toInt();
          m_nCommConnTime = strCommonTime.toInt();
          
          Serial.printf("CHANGE_CONNECT_TIME:  UPDATE:%d  COMMON:%d\n", m_nUpdateConnTime,m_nCommConnTime);

        }
      
      }
      Serial.println("Contents:");
      Serial.println(packetBuffer);
    }

  
}



void 
CMyWifiControl::SetSendInfo(float fSolar, float fBat1, float fBat2, float fEsp32Temp, float fInnerTemp,int nLedOnHour, int nLedOffHour)
{
  //서버 전송을 위한 자료저장//
  m_fSolarVin = fSolar;
  m_fBatVin1  = fBat1;
  m_fBatVin2  = fBat2;
  m_fEsp32Temp= fEsp32Temp;
  m_fInnerTemp= fInnerTemp;

  m_nLedOnHour = nLedOnHour;
  m_nLedOffHour= nLedOffHour;
  //
  time_t ltime;
  time( &ltime);   
  struct tm* pTime = localtime(&ltime);
  char szDate[100];
  sprintf(szDate,"%04d-%02d-%02d %02d:%02d:%02d",
          pTime->tm_year +1900,pTime->tm_mon + 1,pTime->tm_mday,
          pTime->tm_hour,pTime->tm_min,pTime->tm_sec);

  Serial.printf("SET SEND INFO: [%s] Solar:%4.2f  BAT1:%4.2f BAT2:%4.2f ESP:%4.2f  In:%4.2f   On:%d  Off:%d\n", 
                  szDate,fSolar,fBat1,fBat2,fEsp32Temp,fInnerTemp,nLedOnHour,nLedOffHour);
    
}

void 
CMyWifiControl::SetPirControl(CPirControl* pPirControl)
{
  m_pPirControl = pPirControl;  
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
