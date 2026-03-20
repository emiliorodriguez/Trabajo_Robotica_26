#include<Arduino.h>

extern const byte pin_IN1;
extern const byte pin_IN2;
extern const byte pin_ENA;

void driver_init();
void manejo_driver(int dir, float pwm_val, int PWM, int IN1, int IN2);