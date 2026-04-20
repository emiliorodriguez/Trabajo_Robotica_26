#include<Driver_hombro.h>

const byte pin_IN3 = 9;//Pines de interruptores
const byte pin_IN4 = 10;
const byte pin_ENB = 11;//PWM 

void driver_hombro_init()
{
    pinMode(pin_IN3,OUTPUT);
    pinMode(pin_IN4,OUTPUT);
    pinMode(pin_ENB,OUTPUT);
}

void manejo_driver_hombro(int dir, float pwm_val, int PWM, int IN3, int IN4)
{
    analogWrite(PWM,pwm_val);//Velocidad a la que queremos que vaya

    if(dir==1)//Sentido horario
    {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    }
    else if (dir==-1)//Sentido antihorario
    {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
    }
    else//Parado
    {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
}