#include<Electroiman.h>

const byte pin_iman=5;

void iman_init()
{
    pinMode(pin_iman, OUTPUT);
    digitalWrite(pin_iman,LOW);//Por seguridad
}

void activar_iman(int p_iman)   
{
    digitalWrite(pin_iman,HIGH);
}

void desactivar_iman(int p_iman)
{
    digitalWrite(pin_iman,LOW);
}