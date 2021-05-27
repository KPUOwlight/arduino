#ifndef _LED_BATTERY_
#define _LED_BATTERY_

//------------------------------------------------------------------------//
#define _DEBUG_       0
#define _NOUSE_WIFI_  1
//------------------------------------------------------------------------//

#define BAT_CHARGE1  16  //베터리 충전일 때 BAT_CHARGE1 = HIGH LED_LIGHT1 = LOW
#define LED_LIGHT1   17  //PWM 일때 BAT_CHARGE1 = LOW LED_LIGHT1 = HIGH

#define BAT_CHARGE2  5   //베터리 충전일 때 BAT_CHARGE2 = HIGH LED_LIGHT2 = LOW
#define LED_LIGHT2   18  //PWM 일때 BAT_CHARGE2 = LOW LED_LIGHT2 = HIGH


float Esp32Temperature();
float InnerTemperature();


#endif//_LED_BATTERY_
