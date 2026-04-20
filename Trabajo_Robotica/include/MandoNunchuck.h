#include <Arduino.h>
#include <WiiChuck.h>
#include <Wire.h>//I2C
#include <Cinematica.h>
#include <PID_cintura.h>
#include <vector> // Necesario para usar std::vector

// Cambiamos el tipo de retorno de float a std::vector<float>
std::vector<float> manejar_nunchuck_y_cinematica();
void nunchuck_init();
//float manejar_nunchuck_y_cinematica();
void nunchuck_debug_teleplot();
