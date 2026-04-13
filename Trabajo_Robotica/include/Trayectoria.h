#ifndef TRAYECTORIA_H
#define TRAYECTORIA_H

#include <Arduino.h>
#include "Cinematica.h" 

/*
 * Función para generar trayectorias rectas en 3D.
 * Recibe las coordenadas de inicio y fin, el número de pasos,
 * y las medidas físicas del robot.
 */

void trazarLineaRecta(double x_inicio, double y_inicio, double z_inicio, 
                      double x_fin, double y_fin, double z_fin, int pasos,
                      double angulo_ataque, double d1, double d2, double d3, double d4, double h,
                      // Punteros a funciones de lectura de encoders
                      long int (*leer_enc_cintura)(), long int (*leer_enc_hombro)(),
                      // Punteros a funciones de control PID
                      float (*calcular_pid_cintura)(int, int), float (*calcular_pid_hombro)(int, int),
                      // Punteros a la función del driver
                      void (*mover_driver)(int, float, int, int, int),
                      // Pines de los drivers pasados por valor
                      int pin_ENA_cintura, int pin_IN1_cintura, int pin_IN2_cintura,
                      int pin_ENA_hombro, int pin_IN1_hombro, int pin_IN2_hombro);

#endif