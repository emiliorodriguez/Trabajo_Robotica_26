#include <Arduino.h>

extern volatile long int pos_encoder_cintura;//Usamos extern para que el main pueda ver esta variable
const byte pin_interrupcion_A=18;
const byte pin_interrupcion_B=19;

void encoder_cintura_init();
void manejo_interrupcion_cintura();
void print_encoder_cintura();