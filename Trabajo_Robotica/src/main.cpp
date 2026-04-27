#include <Arduino.h>

#include <Encoder_cintura.h>
#include <Driver_cintura.h>
#include <PID_cintura.h>

#include <Encoder_hombro.h>
#include <Driver_hombro.h>
#include <PID_hombro.h>

#include <Servo.h>

#include <Cinematica.h>
#include <Trayectoria.h>

#include <MandoNunchuck.h>

#include <Electroiman.h>


using namespace std;


//Creamos dos servos
Servo servo_codo, servo_muneca;

// Variables para ver si hay cambio en los servos
float ultimo_codo = 90.0;
float ultima_muneca = 0.0;

unsigned long ultimo_tiempo = 0;
const int ciclo_ms = 10; // El robot pensará cada 10ms (100 veces por segundo)

// --- SEGURO DE EMERGENCIA ---
bool emergencia_activa = false;
unsigned long t_bloqueo_cintura = 0;
unsigned long t_bloqueo_hombro = 0;

const int UMBRAL_CINTURA = 40; // 5.4 grados de margen
const int UMBRAL_HOMBRO = 30;  // 4.3 grados de margen (más relajado para 5000 pasos)
const int TIEMPO_MAX_BLOQUEO_CINTURA = 1000;  // 0.5 segundos
const int TIEMPO_MAX_BLOQUEO_HOMBRO = 1000;  // 0.5 segundos


void disparar_emergencia(String motivo);

void setup() 
{
  Serial.begin(115200); // Para ver los pasos en el monitor

  servo_codo.write(90);
  servo_muneca.write(0);
  servo_codo.attach(12);
  servo_muneca.attach(13);

  encoder_cintura_init();
  encoder_hombro_init();
  driver_cintura_init();
  driver_hombro_init();

  nunchuck_init();

  iman_init();
  
}

void loop() 
{

  float pos_objetivo_cintura, pos_objetivo_hombro, objetivo_codo, objetivo_muneca;

  if (millis() - ultimo_tiempo >= ciclo_ms)  //Para no saturar con el nunchuck
  {
    ultimo_tiempo = millis();

    // Si la emergencia ha saltado, bloqueamos todo el pensamiento del robot
    if (emergencia_activa) return;

    manejar_nunchuck_y_cinematica(pos_objetivo_cintura, pos_objetivo_hombro,  objetivo_codo, objetivo_muneca);

    // Solo escribimos al servo si el objetivo ha cambiado realmente
    if (abs(objetivo_codo - ultimo_codo) > 0.5) 
    {
        servo_codo.write((int)objetivo_codo);
        ultimo_codo = objetivo_codo;
    }

    if (abs(objetivo_muneca - ultima_muneca) > 0.5) 
    {
        servo_muneca.write((int)objetivo_muneca);
        ultima_muneca = objetivo_muneca;
    }


    float u = control_pos_cintura(pos_encoder_cintura, pos_objetivo_cintura);
    float u_hombro = control_pos_hombro(pos_encoder_hombro, pos_objetivo_hombro);
  

    //CINTURA
    int dir_cintura = (u >= 0) ?  1 : -1;
    float pwr_cintura = fabs(u);

    // Limitamos la potencia máxima para no forzar el driver
    if (pwr_cintura > 200) pwr_cintura = 200;

    // Zona muerta Cintura
    if (fabs(pos_objetivo_cintura - pos_encoder_cintura) < 10) 
    {
      pwr_cintura = 0; 
    } 
    else if (pwr_cintura > 0 && pwr_cintura < 100) 
    {
      pwr_cintura = 100; 
    }

    // 1. Vigilancia Cintura
    float error_cintura = fabs(pos_objetivo_cintura - pos_encoder_cintura);
    if (pwr_cintura >= 199 && error_cintura > UMBRAL_CINTURA) 
    {
      if (t_bloqueo_cintura == 0) 
      {
        t_bloqueo_cintura = millis();
      }
      if (millis() - t_bloqueo_cintura > TIEMPO_MAX_BLOQUEO_CINTURA) {
        disparar_emergencia("CINTURA BLOQUEADA");
        return;
      }
    } 
    else 
    {
      t_bloqueo_cintura = 0;
    }

    // Enviamos orden Cintura
    manejo_driver_cintura(dir_cintura, pwr_cintura, pin_ENA, pin_IN1, pin_IN2);


    //HOMBRO
    int dir_hombro = (u_hombro >= 0) ? -1 : 1;//Confirmar esto
    float pwr_hombro = fabs(u_hombro);

    if (pwr_hombro > 200) pwr_hombro = 200;

    // Zona muerta Hombro
    if (fabs(pos_objetivo_hombro - pos_encoder_hombro) < 10) 
    {
      pwr_hombro = 0;   
    } 
    else if (pwr_hombro > 0 && pwr_hombro < 100) 
    {
      pwr_hombro = 100; 
    }

    // 2. Vigilancia Hombro
    float error_hombro = fabs(pos_objetivo_hombro - pos_encoder_hombro);
    if (pwr_hombro >= 199 && error_hombro > UMBRAL_HOMBRO) 
    {
      if (t_bloqueo_hombro == 0)
      {
        t_bloqueo_hombro = millis();
      }
      if (millis() - t_bloqueo_hombro > TIEMPO_MAX_BLOQUEO_HOMBRO) {
        disparar_emergencia("HOMBRO BLOQUEADO");
        return;
      }
    } 
    else 
    {
      t_bloqueo_hombro = 0;
    }

    //Enviamos orden hombro
    manejo_driver_hombro(dir_hombro, pwr_hombro, pin_ENB, pin_IN3, pin_IN4);

    nunchuck_debug_teleplot();
    // 1. Posición Objetivo vs Actual (Para ver el error)
    Serial.print(">objetivo_hombro:");
    Serial.println(pos_objetivo_hombro);

    Serial.print(">actual_hombro:");
    Serial.println(pos_encoder_hombro);

    // 2. Potencia aplicada (Para ver si el motor está al límite)
    // Multiplicamos por dir_hombro para ver si la fuerza es hacia arriba o hacia abajo
    Serial.print(">pwr_hombro:");
    Serial.println(pwr_hombro * dir_hombro);

    // 3. Opcional: Ver el error numérico exacto
    Serial.print(">error_hombro:");
    Serial.println(pos_objetivo_hombro - pos_encoder_hombro);

  }

  
}

void disparar_emergencia(String motivo) {
  emergencia_activa = true;
  
  // Apagamos drivers de DC inmediatamente
  manejo_driver_cintura(1, 0, pin_ENA, pin_IN1, pin_IN2);
  manejo_driver_hombro(1, 0, pin_ENB, pin_IN3, pin_IN4);
  
  // Opcional: Detener servos (dejarán de ejercer fuerza)
  servo_codo.detach(); 
  servo_muneca.detach();

  Serial.println("######################################");
  Serial.print("!!! PARADA DE EMERGENCIA: ");
  Serial.println(motivo);
  Serial.println("######################################");
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