#include <Arduino.h>

extern volatile long int pos_encoder;//Usamos extern para que el main pueda ver esta variable
const byte pin_interrupcion_A=2;
const byte pin_interrupcion_B=3;

void encoder_init();
void manejo_interrupcion();
void print_encoder();