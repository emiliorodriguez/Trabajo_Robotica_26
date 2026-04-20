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
#include <iostream>
#include <Vector.h>
#include <Trayectoria.h>
#include <Electroiman.h>
#include <Servo.h>


using namespace std;


// put function declarations here:
Servo servo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Para ver los pasos en el monitor
  encoder_cintura_init();
  encoder_hombro_init();
  driver_cintura_init();
  driver_hombro_init();
  nunchuck_init();
  servo.attach(12);//Hacer una función servo
  //servo.attach(13);
  
}

void loop() {
  // put your main code here, to run repeatedly:
std::vector<float> objetivos = manejar_nunchuck_y_cinematica();

 
  
  /*
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



  // Para movernos con elegancia y no a potencia máxima para cada movimiento con el servo
  for (int pos = 0; pos <= 180; pos += 1) { 
    miServo.write(pos); Aquí decimos, servo vete a esta posición o a esta otra.
    delay(15); // Cuanto más delay, más lento irá
  }*/
       // Llamamos a tu función que lee el Nunchuk y calcula la posición ideal (pasos)
  // Esta función usa el JoyX para incrementar/decrementar la posición del robot
  //float pos_objetivo_pasos = manejar_nunchuck_y_cinematica();
  float pos_objetivo_cintura = objetivos[0];
    float pos_objetivo_hombro  = objetivos[1];
    float objetivo_codo    = objetivos[2];

  // --- FASE 2: CÁLCULO DEL CONTROL (PID) ---
  
  // El PID compara dónde estamos (pos_encoder_cintura) con dónde queremos ir
 float u = control_pos_cintura(pos_encoder_cintura, pos_objetivo_cintura);
float u_hombro = control_pos_hombro(pos_encoder_hombro, pos_objetivo_hombro);

  // --- FASE 3: ACTUACIÓN (MOTORES) ---
  
  int direccion = (u >= 0) ? 1 : -1;
  float potencia = fabs(u);

  // Limitamos la potencia máxima para no forzar el driver
  if (potencia > 100) potencia = 100;

  // Zona muerta: Si la potencia es muy baja, el motor no vence la fricción
  // Si el error es muy pequeño, paramos el motor (deadzone de precisión)
  /*if (fabs(pos_objetivo_pasos - pos_encoder_cintura) < 2) {
    potencia = 0; 
  } 
  else if (potencia > 0 && potencia < 40) {
    potencia = 70; // Bajamos de 100 a 70 para que no sea tan brusco
}
*/
if (fabs(pos_objetivo_cintura - pos_encoder_cintura) < 2) {
    potencia = 0; 
  } 
  else if (potencia > 0 && potencia < 40) {
    potencia = 70; 
  }
  // Enviamos la orden al puente H
  manejo_driver_cintura(direccion, potencia, pin_ENA, pin_IN1, pin_IN2);
  //delay(10);
  

  // --- FASE 4: TELEMETRÍA PARA TELEPLOT ---
  
  // Estas funciones ya tienen el formato ">Nombre:Valor|g" que pide Teleplot
  //print_encoder_cintura(); // Muestra la posición real
  //print_PID_cintura();     // Muestra el Objetivo y la posición Actual superpuestos
  
  // Opcional: Ver la potencia que el PID está aplicando
  //Serial.print(">PWM_Salida:");
  //Serial.print(potencia * direccion);
  //Serial.println("|g");

  // Un pequeño delay para que el bucle sea estable (aprox 100Hz)
  //delay(10);
//parte del hombro
int dir_hombro = (u_hombro >= 0) ? 1 : -1;
  float pwr_hombro = fabs(u_hombro);
  if (pwr_hombro > 100) pwr_hombro = 100;

  // Zona muerta Hombro
  if (fabs(pos_objetivo_hombro - pos_encoder_hombro) < 2) {
    pwr_hombro = 0; 
  } else if (pwr_hombro > 0 && pwr_hombro < 40) {
    pwr_hombro = 70; 
  }
  manejo_driver_hombro(dir_hombro, pwr_hombro, pin_ENB, pin_IN3, pin_IN4);

  // --- TELEMETRÍA ---
  // Puedes alternar qué ves en Teleplot o crear una función que grafique ambos
  print_PID_cintura(); 
  // print_PID_hombro();
  delay(10);
  

  //servo.write(90);
  //manejo_driver_hombro(-1, 100, pin_ENB, pin_IN3, pin_IN4);
}
