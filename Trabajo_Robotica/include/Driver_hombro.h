#include<Arduino.h>

extern const byte pin_IN3;
extern const byte pin_IN4;
extern const byte pin_ENB;

void driver_hombro_init();
void manejo_driver_hombro(int dir, float pwm_val, int PWM, int IN3, int IN4);