 
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
    delay(5);
      if(gps.encode(uart_gps.read()))      // if there is a new valid sentence...
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
}
 
