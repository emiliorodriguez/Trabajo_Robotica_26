#include <Arduino.h>
#include <math.h>

bool calcular_cinematica_inversa(double X, double Y, double Z, double phi_deg, 
                                 double d1, double d2, double d3, double d4, double h, 
                                 double &t1_deg, double &t2_deg, double &t3_deg, double &t4_deg);

void calcular_cinematica_directa(double t1_deg, double t2_deg, double t3_deg, double t4_deg,
                                 double d1, double d2, double d3, double d4, double h,
                                 double &X, double &Y, double &Z);

bool buscar_postura_variable(double X, double Y, double Z, 
                             double d1, double d2, double d3, double d4, double h, 
                             double &t1_deg, double &t2_deg, double &t3_deg, double &t4_deg);