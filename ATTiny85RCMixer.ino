
#include <avr/wdt.h>

#define PORT_M1 0
#define PORT_M2 1

#define PORT_THROTTLE 3
#define PORT_RUD 4
#define DELAY_TIME 10

void setup() {
  pinMode(PORT_M1, OUTPUT);
  pinMode(PORT_M2, OUTPUT);
  //pinMode(PORT_THROTTLE,INPUT);
  //pinMode(PORT_RUD,INPUT);
  wdt_enable(WDTO_1S);
  //analogWrite(PORT_M2,0);
  //analogWrite(PORT_M1,0);

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
  //TCCR0A = out;
  
  out = throttle - rud + 125;
  if(out > 255) out = 255;
  if(out < 0) out = 0;
  analogWrite(PORT_M2,out);
  //TCCR0B = out;
  
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

