#include<PID_cintura.h>

//Valores experimentales de cada constante

/*No toques los tres a la vez o te volverás loco. Sigue este orden:Solo K_p: Pon K_i y K_d a cero. 
Sube K_p hasta que el brazo se mueva con decisión pero empiece a oscilar un poco.
Añade K_d: Sube K_d poco a poco hasta que esa oscilación desaparezca y el brazo se detenga en seco.
Añade K_i: Si ves que el brazo se queda a unos milímetros del objetivo, sube K_i muy despacio para eliminar ese error final.*/

const float Kp=5;
const float Kd=0.5;
const float Ki=0.2;

volatile float pos_actual=0;
float pos_objetivo=0;
float error_previo=0;
float error_integral=0;
long int tiempo_previo=0;
float senal_control=0;

float control_pos_cintura(float actual, float objetivo)
{
    pos_actual=actual;

    long int tiempo_actual=micros();//micros() devuelve los microsegundos desde que la placa empezó a correr

    float diferencia_tiempo=((float)(tiempo_actual-tiempo_previo))/1.0e6;//Calculamos la diferencia de tiempo y lo pasasmos a segundos

    tiempo_previo=tiempo_actual;

    pos_objetivo=objetivo;

    float error=pos_objetivo-pos_actual;//error

    //Parte derivada
    float de_dt=(error-error_previo)/diferencia_tiempo;

    error_previo=error;//actualizamos el error previo

    //Parte integral
    error_integral=error_integral+(error*diferencia_tiempo);//El error se acumula

    //Anti-Windup (Para acumular error hasta un límite). Esto nos sirve para cuando mantenemos el nunchuck mucho tiempo hacia arriba
    //Limitamos a 150 la potencia y si le cuesta al motor lo subimos y si le sobra potencia lo bajamos
    if (error_integral > 150)
    {
        error_integral = 150; 
    }
    if (error_integral < -150)
    {
        error_integral = -150; 
    }

    //PID completo
    if(fabs(error)<10)//Definimos un umbral de error (Evitamos que el PID se obsesione)
    {
        error_integral=0;//Dejamos de acumular error cuando estamos en la zona valida
        return 0;//Y cerramos la función
    }

    return senal_control=(Kp*error)+(Kd*de_dt)+(Ki*error_integral);

}

void print_PID_cintura()//Graficamos con teleplot
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