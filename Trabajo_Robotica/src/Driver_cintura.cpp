#include<Driver_cintura.h>

const byte pin_IN1 = 7;//Pines de interruptores
const byte pin_IN2 = 8;
const byte pin_ENA = 6;//PWM 
 
void driver_cintura_init()
{
    pinMode(pin_IN1,OUTPUT);
    pinMode(pin_IN2,OUTPUT);
    pinMode(pin_ENA,OUTPUT);
}

void manejo_driver_cintura(int dir, float pwm_val, int PWM, int IN1, int IN2)
{
    analogWrite(PWM,pwm_val);//Velocidad a la que queremos que vaya

    if(dir==1)//Sentido horario
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    }
    else if (dir==-1)//Sentido antihorario
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    }
    else//Parado
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
}