#include <Arduino.h>
#include <Encoder.h>
#include <Driver.h>
#include <PID.h>
#include <Cinematica.h>
#include <Wire.h>//I2C
#include <WiiChuck.h>
#include <iostream>
#include "Vector.h"

using namespace std;

// --- MEDIDAS FÍSICAS DEL ROBOT ---
// (Cámbialas aquí por las medidas reales de tus eslabones)
double d1 = 10.0;
double d2 = 15.0;
double d3 = 15.0;
double d4 = 5.0;
double h  = 10.0;
double angulo_ataque = -90.0; // Ángulo phi_deg con el que el rotulador toca el papel

// put function declarations here:

Accessory nunchuck;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); // Para ver los pasos en el monitor
  encoder_init();
  driver_init();

  nunchuck.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  nunchuck.readData();

  int lecturaX = nunchuck.getJoyX();

  // Mapeamos el joystick a un "paso" de movimiento (-5 a 5)
  int paso = map(lecturaX, 35, 215, -5, 5);

  // Si el joystick no está en el centro (zona muerta entre 120 y 135)
  if (lecturaX < 120 || lecturaX > 135) {
      pos_objetivo = pos_objetivo + paso; 
  }

  // Evitamos que el objetivo se salga de los límites físicos (0 a 1200)
  pos_objetivo = constrain(pos_objetivo, 0, 1200);


  print_encoder();
  print_PID();

  float u=control_pos(pos_encoder);

  int dir=1;//Dirección de la señal de control
  if(u<0)
  {
    dir=-1;
  }
  
  float pwr=fabs(u);//Potencia de la señal de control
  if(pwr>255)//Máximo de potencia
  {
    pwr=255;
  }

  if(pwr>0 && pwr < 50) //Minimo de potencia ya que sino no se mueve por fricción (Hay que ir probando experimentalmente)
  {
    pwr = 50;
  }

  manejo_driver(dir, pwr, pin_ENA, pin_IN1, pin_IN2);


}

// Función para generar trayectorias rectas en 3D
void trazarLineaRecta(double x_inicio, double y_inicio, double z_inicio, 
                      double x_fin, double y_fin, double z_fin, int pasos) {
  
  // 1. Calculamos los incrementos para X, Y y Z
  double incremento_x = (x_fin - x_inicio) / pasos;
  double incremento_y = (y_fin - y_inicio) / pasos;
  double incremento_z = (z_fin - z_inicio) / pasos;

  // Bucle para cada micropunto
  for (int i = 1; i <= pasos; i++) {
    
    double x_actual = x_inicio + (incremento_x * i);
    double y_actual = y_inicio + (incremento_y * i);
    double z_actual = z_inicio + (incremento_z * i);

    // Variables donde tu función de Cinematica.h guardará los resultados
    double t1_deg, t2_deg, t3_deg, t4_deg;

    /* ---------------------------------------------------------
       A. LLAMADA A TU CINEMÁTICA INVERSA
       --------------------------------------------------------- */
      bool exito = calcular_cinematica_inversa(x_actual, y_actual, z_actual, angulo_ataque, 
                                             d1, d2, d3, d4, h, 
                                             t1_deg, t2_deg, t3_deg, t4_deg);

    // Si el punto está fuera del alcance físico del robot, nos saltamos este micropunto
    if (!exito) continue; 

    /* ---------------------------------------------------------
       B. CONVERSIÓN DE GRADOS A PASOS DEL ENCODER
       (Ajusta el multiplicador según la resolución de tus encoders)
       --------------------------------------------------------- */
    // Suponiendo que 1 grado = 10 pasos de encoder (cámbialo por tu valor real)
    int pos_objetivo_cintura = t1_deg * 10; 
    int pos_objetivo_hombro = t2_deg * 10;

    /* ---------------------------------------------------------
       C. MOVER LOS SERVOS DEL CODO Y LA MUÑECA DIRECTAMENTE
       (Los servos ya entienden de grados, no necesitan PID)
       --------------------------------------------------------- */
    // miServoCodo.write(t3_deg);
    // miServoMuneca.write(t4_deg);

    /* ---------------------------------------------------------
       D. BUCLE PID PARA LOS MOTORES DC (Cintura y Hombro)
       --------------------------------------------------------- */
    bool micropunto_alcanzado = false;
    
    while (!micropunto_alcanzado) {
      
      // -- MOTOR 1 (Cintura) --
      // ¡OJO! Usa las variables reales que crees para la cintura
      float u_cintura = control_pos(pos_encoder); // Aquí deberás usar pos_encoder_cintura
      int dir_cintura = (u_cintura < 0) ? -1 : 1;
      float pwr_cintura = fabs(u_cintura);
      if(pwr_cintura > 255) pwr_cintura = 255;
      if(pwr_cintura > 0 && pwr_cintura < 50) pwr_cintura = 50; 
      manejo_driver(dir_cintura, pwr_cintura, pin_ENA, pin_IN1, pin_IN2);

      // -- MOTOR 2 (Hombro) --
      // ¡OJO! Aquí tendrás que usar tus nuevas variables duplicadas para el motor 2
      // float u_hombro = control_pos_hombro(pos_encoder_hombro); 
      // int dir_hombro = (u_hombro < 0) ? -1 : 1;
      // ...
      // manejo_driver(dir_hombro, pwr_hombro, pin_ENA_2, pin_IN1_2, pin_IN2_2);

      // -- COMPROBAR LLEGADA --
      // Medimos el error en pasos de encoder
      float error_cintura = pos_objetivo_cintura - pos_encoder; // Usar pos_encoder_cintura
      // float error_hombro = pos_objetivo_hombro - pos_encoder_hombro;
      
      // Cuando ambos motores lleguen a su objetivo, salimos del while
      if (fabs(error_cintura) < 10 /* && fabs(error_hombro) < 10 */) {
        micropunto_alcanzado = true; 
      }
      
      delay(5);
    }
  }
}