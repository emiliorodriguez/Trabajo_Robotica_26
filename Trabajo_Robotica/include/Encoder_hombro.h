#include <Arduino.h>

extern volatile long int pos_encoder_hombro;//Usamos extern para que el main pueda ver esta variable
const byte pin_interrupcion_C=2;
const byte pin_interrupcion_D=3;

void encoder_hombro_init();
void manejo_interrupcion_hombro();
void print_encoder_hombro();