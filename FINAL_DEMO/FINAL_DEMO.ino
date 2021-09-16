#include <DHT.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

DHT DHT(4,DHT11);

#define DHT11_PIN 4

float humi;
float temp;

SoftwareSerial mySerial(2,3); // RX,TX 와이파이 모듈

TinyGPSPlus gps;

//SoftwareSerial uart_gps(7,6); // GPS 모듈 RX, TX

String ssid = "AndroidHotspot7283";
String PASSWORD = "01020137283";
String host = "13.125.177.193";
// ======== 실험을 위한 스마트폰 핫스팟 연결
int dust_sensor = A0; //미세먼지 핀 번호
int sensor_led = 5;   //미세먼지 센서 안에 있는 적외선 LED 핀 번호
float dust_value = 0;
float dustDensityug = 0;
float calcVoltage = 0;

float dust_init = 0;
float dust_initial = 0;

int c;
float lat, lng;

SoftwareSerial uart_gps(7,6); // GPS 모듈 RX, TX

void connectWifi() {  // 와이파이 연결 동작
  String join ="AT+CWJAP=\""+ssid+"\",\""+PASSWORD+"\"";
  
  Serial.println("Connect Wifi...");
  mySerial.println(join);
  delay(10000);
  if(mySerial.find("OK"))
  {
    Serial.print("WIFI connect\n");
  }else
  {
    Serial.println("connect timeout\n");
  }
  delay(1000);
}

void httpclient(String char_input)  // 데이터베이스로 측정한 값을 전송하는 동작
{
  delay(100);
  Serial.println("connect TCP...");
  mySerial.println("AT+CIPSTART=\"TCP\",\""+host+"\",80");
  delay(2000);
  if(Serial.find("ERROR")) return;
          
  Serial.println("Send data...");
  String url=char_input;
  //Serial.println(url);
  String cmd="GET /process4.php?temp="+url+" HTTP/1.0\r\n\r\n";
  //Serial.println(cmd);
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  mySerial.print("AT+CIPSEND=");
  mySerial.println(cmd.length());
  delay(2000);
  mySerial.println(cmd);
  Serial.println(cmd);
  delay(2000);

  if(Serial.find("ERROR")) return;
  //mySerial.println("AT+CIPCLOSE");
  delay(100);
}

void setup(){
  Serial.begin(9600); //  시리얼 모니터 시작, 속도는 9600
  delay(100);
  mySerial.begin(9600);
  delay(100);
  connectWifi(); delay(500);
  Serial.println("Start GPS... ");
  pinMode(sensor_led,OUTPUT); //  미세먼지 적외선 LED를 출력으로 설정

  for(int i = 0; i < 5; i++) {  //  미세먼지 기본값을 정해주기 위한 동작
    digitalWrite(sensor_led, LOW);  // LED 켜기
    delayMicroseconds(280); //  샘플링해주는 시간
    dust_init += analogRead(dust_sensor); //  센서 값 읽어오기
    delayMicroseconds(40);  //  너무 많은 데이터 입력을 피해주기 위해 잠시 멈춰주는 시간
    digitalWrite(sensor_led, HIGH); //  LED끄기
    delayMicroseconds(9680);  //  LED끄고 대기
  }

  dust_initial = (((dust_init/5)*5.0)/1024);
  Serial.print("dust_initial : ");
  Serial.println(dust_initial);
  uart_gps.begin(9600);
  delay(1000);
}

void loop()
{
  while(uart_gps.available() > 0)
  {
    Serial.print(".");
    delay(5);
    if(gps.encode(uart_gps.read()))
    {
      Serial.println(" ");
      if(gps.location.isValid()) {
        Serial.print("Location : ");
        Serial.print(gps.location.lat(),6);
        lat = gps.location.lat();
        Serial.print(", ");
        Serial.println(gps.location.lng(),6);
        lng = gps.location.lng();
      }
      else {
        Serial.println("INVALID");
      }
    }
  }
  
  Serial.println(" ");
  Serial.print("Position : ");
  Serial.print(lat,6); Serial.print(", ");Serial.println(lng,6); 
  
  digitalWrite(sensor_led, LOW);
  delayMicroseconds(280);

  dust_value = analogRead(dust_sensor);
  Serial.print("dust_value: ");
  Serial.println(dust_value);

  delayMicroseconds(40);

  digitalWrite(sensor_led, HIGH);
  delayMicroseconds(9680);

  calcVoltage = dust_value * (5.0 / 1024);
  dustDensityug = ((calcVoltage - dust_initial) / 0.005);
  // 미세먼지 값 계산
  if(dustDensityug < 0) dustDensityug = 0;  // 음수가 나왔을 경우 예외처리
  Serial.print("Dust Density [ug.m^3]: ");
  Serial.println(dustDensityug);

  int val = analogRead(A1);
  Serial.print("READED = ");
  Serial.print(val);
  Serial.print(" , Volt = ");
  int volt = map(val,0,1023,0,100);
  Serial.print(volt);
  Serial.println("mV");
  
  temp = DHT.readTemperature();
  humi = DHT.readHumidity();
  String str_output = String(temp)+"&humi="+String(humi)+"&dust="+String(dustDensityug)+"&bat="+String(volt)+"&lat="+String(lat,5)+"&long="+String(lng,5);
  delay(1000);
  httpclient(str_output);
  delay(1000);

  while (mySerial.available())
  {
    char response = mySerial.read();
    Serial.write(response);
    if(response=='\r') Serial.print('\n');
  }
  Serial.println("\n==================================\n");
  delay(1000);
}
