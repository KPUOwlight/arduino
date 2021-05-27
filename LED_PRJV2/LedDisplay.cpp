#include <Arduino.h>
#include "LedDisplay.h"


void LED_ON(int nPin0, int nPin1)
{
  digitalWrite(nPin0, HIGH);
  digitalWrite(nPin1, HIGH);
}

void LED_OFF(int nPin0, int nPin1)
{
  digitalWrite(nPin0, LOW);
  digitalWrite(nPin1, LOW);
}

void CLEAR_LED()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED1_COM, LOW);
  digitalWrite(LED2_COM, LOW);

}
