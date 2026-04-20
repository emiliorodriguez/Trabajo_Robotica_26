#include <Encoder_hombro.h>

volatile long int pos_encoder_hombro=0;
// 11 PPR * 450 Reducción * 1 (Modo lectura rising) = 4950
const float PPR_TOTAL = 4950.0;

void encoder_hombro_init()
{
    pinMode(pin_interrupcion_C,INPUT_PULLUP);//Usamos input_pullup para no tener el pin en el aire en caso de no enviar nada (así no hay ruido)
    pinMode(pin_interrupcion_D,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_interrupcion_C),manejo_interrupcion_hombro, RISING);//Cuando A tenga flanco de subida
}

void manejo_interrupcion_hombro()//Funcion de la interrupción
{

    int b = digitalRead(pin_interrupcion_D);//Leemos B

    if (b>0) //Giro horario
    {
        pos_encoder_hombro+=1;
    }
    else //Giro antihorario
    {
        pos_encoder_hombro-=1;
    }
    
}

void print_encoder_hombro()
{
    // 1. Leemos los pasos de forma segura para así no mezclar instantes ya que el encoder puede ir más rapido que el Arduino
    noInterrupts();
    long int pasos_actuales = pos_encoder_hombro;
    interrupts();

    float grados = (pasos_actuales * 360.0) / PPR_TOTAL;

    Serial.print("Pasos: ");
    Serial.print(pasos_actuales);
    Serial.print(" | Ángulo: ");
    Serial.print(grados, 2); // Mostramos 2 decimales
    Serial.println("°");
}