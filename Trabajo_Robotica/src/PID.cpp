#include<PID.h>

const float Kp=10;
const float Kd=2;
const float Ki=0;

volatile long int pos_actual=0;
int pos_objetivo=1200;

float error_previo=0;
float error_integral=0;

long int tiempo_previo=0;

float senal_control=0;


float control_pos(int actual)
{
    pos_actual=actual;

    long int tiempo_actual=micros();//micros() devuelve los microsegundos desde que la placa empezó a correr

    float diferencia_tiempo=((float)(tiempo_actual-tiempo_previo))/1.0e6;//Calculamos la diferencia de tiempo y lo pasasmos a segundos

    tiempo_previo=tiempo_actual;

    int error=pos_objetivo-pos_actual;//error

    //Parte derivada
    float de_dt=(error-error_previo)/diferencia_tiempo;

    error_previo=error;//actualizamos el error previo

    //Parte integral
    error_integral=error_integral+(error*diferencia_tiempo);//El error se acumula

    //PID completo
    if(fabs(error)<2)//Definimos un umbral de error (Evitamos que el PID se obsesione)
    {
        error_integral=0;//Dejamos de acumular error cuando estamos en la zona valida
        return 0;//Y cerramos la función
    }

    return senal_control=(Kp*error)+(Kd*de_dt)+(Ki*error_integral);

}

void print_PID()//Graficamos con teleplot
{
    /*Serial.print("Objetivo: ");
    Serial.print(pos_objetivo);
    Serial.print(" | Actual: ");
    Serial.println(pos_actual);*/

    // Formato para Teleplot: >Nombre:Valor|g
    
    // 1. Enviamos el Objetivo (Setpoint)
    Serial.print(">Objetivo:");
    Serial.print(pos_objetivo);
    Serial.println("|g");

    // 2. Enviamos la Posición Actual
    Serial.print(">Actual:");
    Serial.print(pos_actual);
    Serial.println("|g");
}