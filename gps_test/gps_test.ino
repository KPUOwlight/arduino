 
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
 
#define RXPIN 7
#define TXPIN 6

#define GPSBAUD 9600

TinyGPSPlus gps;

SoftwareSerial uart_gps(RXPIN, TXPIN);

float lat,lng;

void setup()
{

  Serial.begin(9600);
  
  uart_gps.begin(GPSBAUD);
  
  Serial.println("");
  Serial.println("GPS Shield QuickStart Example Sketch v12");
  Serial.println("       ...waiting for lock...           ");
  Serial.println("");
}

void loop()
{
  while(uart_gps.available() > 0)     // While there is data on the RX pin...
  {
    Serial.print(".");
      if(gps.encode(uart_gps.read()))      // if there is a new valid sentence...
      {
        Serial.println(" ");
    /*  gps.f_get_position(&lat,&lon);
      Serial.print("Position : ");
      Serial.print(lat); Serial.print(", ");Serial.println(lon);  */

      if(gps.location.isValid()) {
        Serial.print("Location : ");
        Serial.print(gps.location.lat(),3);
        lat = gps.location.lat();
        Serial.print(", ");
        Serial.println(gps.location.lng(),3);
        lng = gps.location.lng();
      }
      else {
        Serial.println("INVALID");
      }
      }   
  }
    Serial.println(" ");
  Serial.print("Position : ");
  Serial.print(lat); Serial.print(", ");Serial.println(lng); 
}
 
