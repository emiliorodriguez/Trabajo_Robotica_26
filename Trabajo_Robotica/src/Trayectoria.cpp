#include "Trayectoria.h"

void trazarLineaRecta(double x_inicio, double y_inicio, double z_inicio, 
                      double x_fin, double y_fin, double z_fin, int pasos,
                      double angulo_ataque, double d1, double d2, double d3, double d4, double h,
                      long int (*leer_enc_cintura)(), long int (*leer_enc_hombro)(),
                      float (*calcular_pid_cintura)(int, int), float (*calcular_pid_hombro)(int, int),
                      void (*mover_driver)(int, float, int, int, int),
                      int pin_ENA_cintura, int pin_IN1_cintura, int pin_IN2_cintura,
                      int pin_ENA_hombro, int pin_IN1_hombro, int pin_IN2_hombro) {
  
  double incremento_x = (x_fin - x_inicio) / pasos;
  double incremento_y = (y_fin - y_inicio) / pasos;
  double incremento_z = (z_fin - z_inicio) / pasos;

  for (int i = 1; i <= pasos; i++) {
    
    double x_actual = x_inicio + (incremento_x * i);
    double y_actual = y_inicio + (incremento_y * i);
    double z_actual = z_inicio + (incremento_z * i);

    double t1_deg, t2_deg, t3_deg, t4_deg;

    bool exito = calcular_cinematica_inversa(x_actual, y_actual, z_actual, angulo_ataque, 
                                             d1, d2, d3, d4, h, 
                                             t1_deg, t2_deg, t3_deg, t4_deg);

    if (!exito) continue; 

    // Ajusta el multiplicador "10" según la resolución real de tus encoders
    int pos_objetivo_cintura = t1_deg * 10; 
    int pos_objetivo_hombro = t2_deg * 10;

    bool micropunto_alcanzado = false;
    
    while (!micropunto_alcanzado) {
      
      // 1. Leer posiciones actuales a través de los punteros a función
      long int pos_actual_cintura = leer_enc_cintura();
      long int pos_actual_hombro = leer_enc_hombro();

      // 2. Calcular PIDs a través de los punteros a función
      float u_cintura = calcular_pid_cintura(pos_objetivo_cintura, pos_actual_cintura);
      float u_hombro = calcular_pid_hombro(pos_objetivo_hombro, pos_actual_hombro);

      // 3. Lógica de dirección y potencia (Cintura)
      int dir_cintura = (u_cintura < 0) ? -1 : 1;
      float pwr_cintura = fabs(u_cintura);
      if(pwr_cintura > 255) pwr_cintura = 255;
      if(pwr_cintura > 0 && pwr_cintura < 50) pwr_cintura = 50; 

      // 4. Lógica de dirección y potencia (Hombro)
      int dir_hombro = (u_hombro < 0) ? -1 : 1;
      float pwr_hombro = fabs(u_hombro);
      if(pwr_hombro > 255) pwr_hombro = 255;
      if(pwr_hombro > 0 && pwr_hombro < 50) pwr_hombro = 50;

      // 5. Mover motores usando el puntero a la función del driver
      mover_driver(dir_cintura, pwr_cintura, pin_ENA_cintura, pin_IN1_cintura, pin_IN2_cintura);
      // Descomenta esto cuando tengas el segundo driver configurado
      // mover_driver(dir_hombro, pwr_hombro, pin_ENA_hombro, pin_IN1_hombro, pin_IN2_hombro);

      // 6. Comprobar llegada
      float error_cintura = pos_objetivo_cintura - pos_actual_cintura;
      // float error_hombro = pos_objetivo_hombro - pos_actual_hombro;
      
      if (fabs(error_cintura) < 10 /* && fabs(error_hombro) < 10 */) {
        micropunto_alcanzado = true; 
      }
      
      delay(5);
    }
  }
}