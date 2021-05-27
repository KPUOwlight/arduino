#include <Arduino.h>
#include "LedBattery.h"


#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();


int ThermistorPin = 27;
#define R0 10000
#define T0 25   //room temperature
#define B  4200 //the coefficient of the thermistor
#define SERISR 10000 //seris resistor 10K



//ESP32 온도
float Esp32Temperature()
{
  // Convert raw temperature in F to Celsius degrees
  float fEsp32Temp = (temprature_sens_read() - 32) / 1.8 ;

#if _DEBUG_	

	Serial.print("Temperature: ");
  Serial.print(fEsp32Temp);
  Serial.println(" C");

#endif
	return fEsp32Temp;

}

float InnerTemperature()
{
	
  //내부 온도 측정//
  int Vo = analogRead(ThermistorPin); 
  if(Vo == 0) Vo = 1;
  
  float thermistorRValue;
  thermistorRValue = 4095.0 / (float)Vo - 1;
  thermistorRValue = SERISR / thermistorRValue;
  
  float temperature;
  temperature = thermistorRValue / R0; // R/R0
  temperature = log(temperature);
  temperature = temperature / B;
  temperature = temperature + 1.0 / (T0 + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15 ;// kelvins to C

#if _DEBUG_ 
  Serial.printf("Temperature(%d): ",Vo);   
  Serial.print(temperature);  
  Serial.println(" C");     
#endif
  return 	temperature;
	
}
