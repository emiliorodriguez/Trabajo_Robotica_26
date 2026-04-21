#include<PID_hombro.h>

const float Kp_hombro=25;
const float Kd_hombro=0.5;
const float Ki_hombro=5.0;

volatile float pos_actual_hombro=0;
float pos_objetivo_hombro=0;

float error_previo_hombro=0;
float error_integral_hombro=0;

long int tiempo_previo_hombro=0;

float senal_control_hombro=0;


float control_pos_hombro(float actual, float objetivo)
{
    pos_actual_hombro=actual;

    long int tiempo_actual=micros();//micros() devuelve los microsegundos desde que la placa empezó a correr

    float diferencia_tiempo=((float)(tiempo_actual-tiempo_previo_hombro))/1.0e6;//Calculamos la diferencia de tiempo y lo pasasmos a segundos

    tiempo_previo_hombro=tiempo_actual;

    pos_objetivo_hombro=objetivo;

    float error=pos_objetivo_hombro-pos_actual_hombro;//error

    //Parte derivada
    float de_dt=(error-error_previo_hombro)/diferencia_tiempo;

    error_previo_hombro=error;//actualizamos el error previo

    //Parte integral
    error_integral_hombro=error_integral_hombro+(error*diferencia_tiempo);//El error se acumula

    //PID completo
    if(fabs(error)<2)//Definimos un umbral de error (Evitamos que el PID se obsesione)
    {
        error_integral_hombro=0;//Dejamos de acumular error cuando estamos en la zona valida
        return 0;//Y cerramos la función
    }

    return senal_control_hombro=(Kp_hombro*error)+(Kd_hombro*de_dt)+(Ki_hombro*error_integral_hombro);

}

void print_PID_hombro()//Graficamos con teleplot
{
    /*Serial.print("Objetivo: ");
    Serial.print(pos_objetivo);
    Serial.print(" | Actual: ");
    Serial.println(pos_actual);*/

    // Formato para Teleplot: >Nombre:Valor|g
    
    // 1. Enviamos el Objetivo (Setpoint)
    Serial.print(">Objetivo:");
    Serial.print(pos_objetivo_hombro);
    Serial.println("|g");

    // 2. Enviamos la Posición Actual
    Serial.print(">Actual:");
    Serial.print(pos_actual_hombro);
    Serial.println("|g");
}