#include <Arduino.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//int calculo_angulos (double x, double y, double z);
bool calcular_cinematica_inversa(double X, double Y, double Z, double phi_deg, 
                                 double d1, double d2, double d3, double d4, double h, 
                                 double &t1_deg, double &t2_deg, double &t3_deg, double &t4_deg);