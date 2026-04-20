#include <Encoder_cintura.h>

volatile long int pos_encoder_cintura=0;
// 11 PPR * 450 Reducción * 1 (Modo lectura rising) = 4950
const float PPR_TOTAL = 4950.0;

void encoder_cintura_init()
{
    pinMode(pin_interrupcion_A,INPUT_PULLUP);//Usamos input_pullup para no tener el pin en el aire en caso de no enviar nada (así no hay ruido)
    pinMode(pin_interrupcion_B,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_interrupcion_A),manejo_interrupcion_cintura, RISING);//Cuando A tenga flanco de subida
}

void manejo_interrupcion_cintura()//Funcion de la interrupción
{

    int b = digitalRead(pin_interrupcion_B);//Leemos B

    if (b>0) //Giro horario
    {
        pos_encoder_cintura+=1;
    }
    else //Giro antihorario
    {
        pos_encoder_cintura-=1;
    }
    
}

void print_encoder_cintura()
{
    // 1. Leemos los pasos de forma segura para así no mezclar instantes ya que el encoder puede ir más rapido que el Arduino
    noInterrupts();
    long int pasos_actuales = pos_encoder_cintura;
    interrupts();

    float grados = (pasos_actuales * 360.0) / PPR_TOTAL;

    Serial.print("Pasos: ");
    Serial.print(pasos_actuales);
    Serial.print(" | Ángulo: ");
    Serial.print(grados, 2); // Mostramos 2 decimales
    Serial.println("°");
}