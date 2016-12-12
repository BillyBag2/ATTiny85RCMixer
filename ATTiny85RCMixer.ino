// Inputs
// ------
// Rudder and throttle are servo inputs where a 1us pulse is 0% and a 2us pulse is 100%
// * Note that for some transmitters you may have to increase the throw setting.
//
// Outputs
// -------
// M1 and M2 are high speed PWM outputs designed to go to a MOSFET then a motor. 
// * A H-bridge would work but this mixer is for single direction motors.
// M1 and M2 are not designated as left or right. In practice the transmitter's rudder direction can easily be swaped.
//
// Watchdog
// --------
// If a pulse of acceptable duration does not appear the Watchdog timer will fire. This is crude fail safe. For example motors will stop 1 second after reception is lost.
//
// Blocking, servo reading, main thread.
// -------------------------------------
// Input is read by the main thread using blocking functions. If a pulse is not an acceptable length it is re-read.
// The watchdog will fire if this happens for more than 1 second.
// Imputs are read alteratly. Not every pulse from the reciever is used.
//
// PWM output
// ----------
// PWM output is via the PWM hardware. It is set to the highest rate posible.
//
// Suggested pin out
//  
// 1 Reset    -|   |- 8 +V
// 2 Throttle -|   |- 7 N/C
// 3 Rudder   -|   |- 6 M2
// 4 0V       -|   |- 5 M1
//

#include <avr/wdt.h>

#define PORT_M1 0
#define PORT_M2 1

#define PORT_THROTTLE 3
#define PORT_RUD 4
#define DELAY_TIME 10

void setup() {
  pinMode(PORT_M1, OUTPUT);
  pinMode(PORT_M2, OUTPUT);

  wdt_enable(WDTO_1S); // WatchDog setup

  pwm_init();

}

void loop() {
  unsigned long duration1,duration2;
  signed long throttle;
  signed long rud;

  signed long out;

  
  duration1 = servo_read(PORT_THROTTLE);
  duration2 = servo_read(PORT_RUD);
  
  
  throttle = conv(duration1);
  rud = conv(duration2);

  out = throttle + rud - 125;
  if(out > 255) out = 255;
  if(out < 0) out = 0;
  analogWrite(PORT_M1,out);
  
  out = throttle - rud + 125;
  if(out > 255) out = 255;
  if(out < 0) out = 0;
  analogWrite(PORT_M2,out);
  
  wdt_reset();
}

unsigned long servo_read(int port)
{
  unsigned long duration;
  noInterrupts();
  duration = pulseIn(port,HIGH);
  interrupts();
  
  while(duration < 900 || duration > 2100)
  {
    noInterrupts();
    duration = pulseIn(port,HIGH);
    interrupts();
  }
  return duration;
}

signed int conv(unsigned long duration)
{
  if(duration < 1000) return 0;

  if(duration > 2000) return 250;

  return (duration - 1000)/4;
}

void pwm_init(void)
{
  TIMSK = 0;
  TIFR = 0;
  OCR0A = 0;
  OCR0B = 0;
  TCCR0A = 3 << WGM00 | 2 << COM0B0 | 2 << COM0A0;
  TCCR0B = 1 << CS00 |  0 << WGM02;
  GTCCR = 0;
}

