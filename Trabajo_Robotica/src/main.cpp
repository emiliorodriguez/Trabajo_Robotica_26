#include <Arduino.h>
#include <Encoder.h>
#include <Driver.h>
#include <PID.h>
#include <Cinematica.h>
#include <Wire.h>//I2C
#include <WiiChuck.h>
#include <iostream>
#include "Vector.h"
#include "Trayectoria.h"

using namespace std;


// put function declarations here:

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

// --- POSICIÓN CARTESIANA ACTUAL DEL ROBOT ---
double robot_x = 15.0; // Sustituye esto por la posición real inicial (en reposo)
double robot_y = 0.0;
double robot_z = 10.0;
double angulo_lienzo_deg = 45.0; // Grados de inclinación del papel hacia atrás

// --- FUNCIÓN PARA LEER NUNCHUCK Y APLICAR TRANSFORMACIONES ---
void manejar_nunchuck_y_cinematica() {
  nunchuck.readData();

  int lecturaX = nunchuck.getJoyX();
  int lecturaY = nunchuck.getJoyY();
  bool botonZ = nunchuck.getButtonZ();
  bool botonC = nunchuck.getButtonC();

  // 1. Determinar movimiento en el sistema de coordenadas local del PAPEL
  double velocidad = 0.5; // mm por iteración del bucle (ajusta para dar más/menos fluidez)
  double delta_p_x = 0; // Movimiento horizontal en el papel
  double delta_p_y = 0; // Movimiento vertical en el papel
  double delta_p_z = 0; // Movimiento perpendicular al papel (acercar/alejar rotulador)

  // Eje X del papel (izquierda/derecha usando JoyX)
  if (lecturaX < 120) delta_p_x = -velocidad;
  else if (lecturaX > 135) delta_p_x = velocidad;

  // Eje Y del papel (arriba/abajo usando JoyY)
  if (lecturaY < 120) delta_p_y = -velocidad;
  else if (lecturaY > 135) delta_p_y = velocidad;

  // Eje Z del papel (entrar/salir usando botones)
  if (botonZ) delta_p_z = -velocidad; // Botón Z: Acercar el rotulador al papel
  else if (botonC) delta_p_z = velocidad; // Botón C: Alejar el rotulador del papel

  // 2. Matriz de Transformación (Rotación sobre el eje Y para el plano inclinado)
  double angulo_rad = angulo_lienzo_deg * (M_PI / 180.0);

  double delta_robot_x = delta_p_y * cos(angulo_rad) - delta_p_z * sin(angulo_rad);
  double delta_robot_y = delta_p_x;
  double delta_robot_z = delta_p_y * sin(angulo_rad) + delta_p_z * cos(angulo_rad);

  // 3. Si hay movimiento, comprobamos si la nueva posición es alcanzable
  if (delta_robot_x != 0 || delta_robot_y != 0 || delta_robot_z != 0) {
      double nuevo_x = robot_x + delta_robot_x;
      double nuevo_y = robot_y + delta_robot_y;
      double nuevo_z = robot_z + delta_robot_z;
      
      double t1_deg, t2_deg, t3_deg, t4_deg;
      
      bool exito = calcular_cinematica_inversa(nuevo_x, nuevo_y, nuevo_z, angulo_ataque, 
                                             d1, d2, d3, d4, h, 
                                             t1_deg, t2_deg, t3_deg, t4_deg);
                                             
      if (exito) {
          robot_x = nuevo_x;
          robot_y = nuevo_y;
          robot_z = nuevo_z;
          pos_objetivo = t1_deg * (845.0 / 360.0); 
      }
  }
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); // Para ver los pasos en el monitor
  encoder_init();
  driver_init();

  nunchuck.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  
  // --- FASE 1: SENSADO Y CÁLCULO (PENSAR) ---
  // Lee el mando, calcula la cinemática y actualiza "pos_objetivo" con el destino final.
  manejar_nunchuck_y_cinematica();

  // Evitamos que el objetivo se salga de los límites físicos (0 a 1200)
  pos_objetivo = constrain(pos_objetivo, 0, 1200);

  print_encoder();
  print_PID();

  // --- FASE 2: CONTROL Y EJECUCIÓN (ACTUAR) ---
  // El PID coge "pos_objetivo" y mueve los motores físicos para llegar a esa meta.
  float u=control_pos(pos_encoder);

  int dir=1;//Dirección de la señal de control
  if(u<0) {
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
