#include <Arduino.h>
#include <WiiChuck.h>
#include <Wire.h>//I2C
#include <Cinematica.h>
#include <Electroiman.h>

void manejar_nunchuck_y_cinematica(float &pasos_cintura, float &pasos_hombro, float &val_codo, float &val_muneca);
void nunchuck_init();
void nunchuck_debug_teleplot();
