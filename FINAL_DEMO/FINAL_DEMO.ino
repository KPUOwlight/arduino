#include <DHT.h>
#include <SoftwareSerial.h>

DHT DHT(4,DHT11);

#define DHT11_PIN 4

float humi;
float temp;

SoftwareSerial mySerial(2,3); // RX,TX


String ssid = "AndroidHotspot7283";
String PASSWORD = "01020137283";
String host = "13.125.177.193";
// ======== 실험을 위한 스마트폰 핫스팟 연결
int dust_sensor = A0; //Connect dust sensor to Arduino A0 pin
int sensor_led = 5;   //Connect 3 led driver pins of dust sensor to Arduino D2
float dust_value = 0;
float dustDensityug = 0;
float calcVoltage = 0;

float dust_init = 0;
float dust_initial = 0;

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
  delay(5000);
  if(Serial.find("ERROR")) return;
          
  Serial.println("Send data...");
  String url=char_input;
  //Serial.println(url);
  String cmd="GET /process3.php?temp="+url+" HTTP/1.0\r\n\r\n";
  //Serial.println(cmd);
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  mySerial.print("AT+CIPSEND=");
  mySerial.println(cmd.length());
  delay(5000);
  mySerial.println(cmd);
  Serial.println(cmd);
  delay(5000);

  if(Serial.find("ERROR")) return;
  //mySerial.println("AT+CIPCLOSE");
  delay(100);
}

void setup(){
  Serial.begin(9600);
  mySerial.begin(9600);
  connectWifi(); delay(500);
  pinMode(sensor_led,OUTPUT);

  for(int i = 0; i < 5; i++) {
    digitalWrite(sensor_led, LOW);
    delayMicroseconds(280);
    dust_init += analogRead(dust_sensor);
    delayMicroseconds(40);
    digitalWrite(sensor_led, HIGH);
    delayMicroseconds(9680);
  }

  dust_initial = (((dust_init/5)*5.0)/1024);
  Serial.print("dust_initial : ");
  Serial.println(dust_initial);
}

void loop()
{
  digitalWrite(sensor_led, LOW);
  delayMicroseconds(280);

  dust_value = analogRead(dust_sensor);

  delayMicroseconds(40);

  digitalWrite(sensor_led, HIGH);
  delayMicroseconds(9680);

  calcVoltage = dust_value * (5.0 / 1024);
  dustDensityug = ((calcVoltage - dust_initial) / 0.005);
  if(dustDensityug < 0) dustDensityug = 0;
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
  String str_output = String(temp)+"&humi="+String(humi)+"&dust="+String(dustDensityug)+"&bat="+String(volt);
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
  delay(2000);
}
