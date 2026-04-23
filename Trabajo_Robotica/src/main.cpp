#include <Arduino.h>
#include <Encoder_cintura.h>
#include <Driver_cintura.h>
#include <Encoder_hombro.h>
#include <Driver_hombro.h>
#include <PID_cintura.h>
#include<PID_hombro.h>
#include <Cinematica.h>
//#include <WiiChuck.h>
#include <MandoNunchuck.h>
//#include <iostream>
//#include <Vector.h>
#include <Trayectoria.h>
#include <Electroiman.h>
#include <Servo.h>


using namespace std;


// put function declarations here:
Servo servo_codo, servo_muneca;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Para ver los pasos en el monitor

  servo_codo.attach(12);//Hacer una función servo
  servo_muneca.attach(13);

  encoder_cintura_init();
  encoder_hombro_init();
  driver_cintura_init();
  driver_hombro_init();
  nunchuck_init();
  iman_init();
  
  
}

// Añade estas variables arriba del loop (como estáticas o globales)
float ultimo_codo = 90.0;
float ultima_muneca = 90.0;

void loop() {

std::vector<float> objetivos = manejar_nunchuck_y_cinematica();

  float pos_objetivo_cintura = objetivos[0];
  float pos_objetivo_hombro  = objetivos[1];
  float objetivo_codo    = objetivos[2];
  float objetivo_muneca = objetivos[3];

  // Escribir a los servos
  //servo_codo.write((int)objetivo_codo);
  //servo_muneca.write((int)objetivo_muneca);

  // SOLO escribimos al servo si el objetivo ha cambiado realmente
    if (abs(objetivo_codo - ultimo_codo) > 0.5) {
        servo_codo.write((int)objetivo_codo);
        ultimo_codo = objetivo_codo;
    }

    if (abs(objetivo_muneca - ultima_muneca) > 0.5) {
        servo_muneca.write((int)objetivo_muneca);
        ultima_muneca = objetivo_muneca;
    }

  float u = control_pos_cintura(pos_encoder_cintura, pos_objetivo_cintura);
  float u_hombro = control_pos_hombro(pos_encoder_hombro, pos_objetivo_hombro);
  


  //CINTURA
  int direccion = (u >= 0) ? -1 : 1;
  float potencia = fabs(u);

  // Limitamos la potencia máxima para no forzar el driver
  if (potencia > 255) potencia = 255;

  // Zona muerta Cintura
  if (fabs(pos_objetivo_cintura - pos_encoder_cintura) < 10) {
    potencia = 0; 
  } 
  else if (potencia > 0 && potencia < 100) {
    potencia = 100; 
  }
  // Enviamos orden Cintura
  manejo_driver_cintura(direccion, potencia, pin_ENA, pin_IN1, pin_IN2);



  //HOMBRO
  int dir_hombro = (u_hombro >= 0) ? -1 : 1;
  float pwr_hombro = fabs(u_hombro);

  if (pwr_hombro > 255) pwr_hombro = 255;

  // Zona muerta Hombro
  if (fabs(pos_objetivo_hombro - pos_encoder_hombro) < 20) {
    pwr_hombro = 0;   
  } else if (pwr_hombro > 0 && pwr_hombro < 100) {
    pwr_hombro = 100; 
  }
  //Enviamos orden hombro
  manejo_driver_hombro(dir_hombro, pwr_hombro, pin_ENB, pin_IN3, pin_IN4);



  // 1. Comparamos el Objetivo vs la Posición Real del Encoder
    // En Teleplot verás dos líneas que deben encontrarse
    Serial.print(">Cintura_Objetivo:");
    Serial.print(pos_objetivo_cintura); // El valor fijo que pusiste para la prueba
    Serial.println("|g");

    Serial.print(">Cintura_Actual:");
    Serial.print(pos_encoder_cintura); // Variable definida en Encoder_hombro.cpp
    Serial.println("|g");

    // 2. Monitoreamos la potencia que el PID está pidiendo
    Serial.print(">Cintura_Power_Output:");
    Serial.print(potencia); // Variable calculada en main.cpp
    Serial.println("|g");

    nunchuck_debug_teleplot();

    

  delay(8); //Editar esto para control Nunchuck (si se baja hay mas PID)

  /* Manejo Electroiman

  activar_iman(pin_iman);
  delay(3000);
  desactivar_iman(pin_iman);

  */

  /* Manejo Servos
  
  servo1.write(90);
  delay(1000);
  servo2.write(90);
  delay(1000);
  servo1.write(-90);
  delay(1000);
  servo2.write(-90);
  delay(1000);

  */
  
}







/* Cosas de servo:

// Para movernos con elegancia y no a potencia máxima para cada movimiento con el servo
  for (int pos = 0; pos <= 180; pos += 1) { 
    miServo.write(pos); Aquí decimos, servo vete a esta posición o a esta otra.
    delay(15); // Cuanto más delay, más lento irá
  }


*/


/* Cosas antiguas potencia y demás:

  // --- FASE 1: SENSADO Y CÁLCULO (PENSAR) ---
  // Lee el mando, calcula la cinemática y actualiza "pos_objetivo" con el destino final.
  float pos_final=manejar_nunchuck_y_cinematica();
  
  // Evitamos que el objetivo se salga de los límites físicos (0 a 1200)
  //pos_objetivo = constrain(pos_objetivo, 0, 1200);

  print_encoder_cintura();
  print_PID_cintura();

  // --- FASE 2: CONTROL Y EJECUCIÓN (ACTUAR) ---
  // El PID coge "pos_objetivo" y mueve los motores físicos para llegar a esa meta.
  float u=control_pos_cintura(pos_encoder_cintura, 1200);

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
    pwr = 100;
  }

  manejo_driver_cintura(dir, pwr, pin_ENA, pin_IN1, pin_IN2);




*/