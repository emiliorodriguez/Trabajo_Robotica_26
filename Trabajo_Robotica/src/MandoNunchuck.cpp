#include <MandoNunchuck.h>

// --- MEDIDAS FÍSICAS DEL ROBOT ---
// (Cámbialas aquí por las medidas reales de tus eslabones)
/*double d1 = 10.0;
double d2 = 12.0;
double d3 = 12.0;
double d4 = 7.8;
double h  = 10.0;
double angulo_ataque = -90.0; // Ángulo phi_deg con el que el rotulador toca el papel

// put function declarations here:



// --- POSICIÓN CARTESIANA ACTUAL DEL ROBOT ---
double robot_x = 15.0; // Sustituye esto por la posición real inicial (en reposo)
double robot_y = 0.0;
double robot_z = 10.0;
double angulo_lienzo_deg = 45.0;  Grados de inclinación del papel hacia atrás

Accessory nunchuck;

void nunchuck_init()
{
    nunchuck.begin();
}

float pasos_actuales_cintura = 0;
float pasos_cintura = 0;
float pasos_hombro = 0;
float pasos_codo = 0;

 --- FUNCIÓN PARA LEER NUNCHUCK Y APLICAR TRANSFORMACIONES ---
float manejar_nunchuck_y_cinematica() {
std::vector<float> manejar_nunchuck_y_cinematica() {
  nunchuck.readData();

  int lecturaX = nunchuck.getJoyX();
  int lecturaY = nunchuck.getJoyY();
  bool botonZ = nunchuck.getButtonZ();
  bool botonC = nunchuck.getButtonC();

  // 1. Determinar movimiento en el sistema de coordenadas local del PAPEL
  double velocidad = 0.1; // mm por iteración del bucle (ajusta para dar más/menos fluidez)
  double delta_p_x = 0; // Movimiento horizontal en el papel
  double delta_p_y = 0; // Movimiento vertical en el papel
  double delta_p_z = 0; // Movimiento perpendicular al papel (acercar/alejar rotulador)

   Eje X del papel (izquierda/derecha usando JoyX)
  if (lecturaX < 120) delta_p_x = -velocidad;
  else if (lecturaX > 135) delta_p_x = velocidad;

  // Eje Y del papel (arriba/abajo usando JoyY)
  if (lecturaY < 120) delta_p_y = -velocidad;
  else if (lecturaY > 135) delta_p_y = velocidad;

  if (abs(lecturaX - 128) > 50) {
      delta_p_x = ((lecturaX - 128) / 128.0) * velocidad;
  }else {
    delta_p_x = 0; // Forzamos el 0 si está en el centro
    }
  
  if (abs(lecturaY - 128) > 50) {
      delta_p_y = ((lecturaY - 128) / 128.0) * velocidad;
  }

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
                                             
      if (exito) {//Hay que poner exito en el parentesis
          robot_x = nuevo_x;
          robot_y = nuevo_y;
          robot_z = nuevo_z;
          //pasos_actuales_cintura = t1_deg * (4950.0 / 360.0); 
          pasos_cintura = t1_deg * (4950.0 / 360.0);
          pasos_hombro  = t2_deg * (4950.0 / 360.0); // Ajusta la constante si el hombro es distinto
          pasos_codo    = t3_deg * (4950.0 / 360.0); // Ajusta según el motor del codo
      }
  }
 // return pasos_actuales_cintura;
 return {pasos_cintura, pasos_hombro, pasos_codo};
}

void nunchuck_debug_teleplot() {
    nunchuck.readData(); // Lee los datos del mando

    // Enviamos el Joystick X e Y
    Serial.print(">JoyX:");
    Serial.print(nunchuck.getJoyX());
    Serial.println("|g");

    Serial.print(">JoyY:");
    Serial.print(nunchuck.getJoyY());
    Serial.println("|g");

    // Enviamos los botones (como 0 o 1)
    Serial.print(">Boton_Z:");
    Serial.print(nunchuck.getButtonZ() ? 1 : 0);
    Serial.println("|g");

    Serial.print(">Boton_C:");
    Serial.print(nunchuck.getButtonC() ? 1 : 0);
    Serial.println("|g");
}*/




// --- VARIABLES DE ESTADO (MEMORIA DEL ROBOT) ---
// Guardamos los ángulos actuales para que el robot se quede quieto al soltar el mando
double ang_cintura = 0.0;
double ang_hombro  = 0.0;
double ang_codo    = 90.0; // Los servos suelen iniciar en el centro (90°)
double ang_muneca  = 90.0;

// Configuración del mando
const int CENTRO_X = 121;
const int CENTRO_Y = 131;
const int DEADZONE = 10;    // Solo se mueve cuando el joystick está casi al límite
const float SENSIBILIDAD_MUNECA = 5; // Grados que aumenta por cada ciclo (ajusta para más/menos velocidad)
const float SENSIBILIDAD_CODO = 3;
const float SENSIBILIDAD_HOMBRO = 2.5;
const float SENSIBILIDAD_CINTURA = 2.5;

Accessory nunchuck;

void nunchuck_init() {
    nunchuck.begin();
}

std::vector<float> manejar_nunchuck_y_cinematica() {
    nunchuck.readData();

    int joyX = nunchuck.getJoyX();
    int joyY = nunchuck.getJoyY();
    bool btnC = nunchuck.getButtonC();
    bool btnZ = nunchuck.getButtonZ();

    // --- 1. MOVIMIENTO DE LA CINTURA (Motor DC) ---
    // Se mueve con el eje X del Joystick
    if (abs(joyX - CENTRO_X) > DEADZONE) {
        // Calculamos cuánto sumar (positivo o negativo)
        double delta = ((joyX - CENTRO_X) / 100.0) * SENSIBILIDAD_CINTURA;
        ang_cintura += delta;
    }

    // --- 2. MOVIMIENTO DEL RESTO (Hombro, Codo, Muñeca) ---
    // Usamos el eje Y del Joystick y los botones como selectores
    if (abs(joyY - CENTRO_Y) > DEADZONE) {
        double delta = ((joyY - CENTRO_Y) / 96.0);

        if (btnC) {
            // CODO (Servo): Botón C + JoyY
            ang_codo = constrain(ang_codo - (delta*SENSIBILIDAD_CODO), 0, 180);
        } 
        else if (btnZ) {
            // MUÑECA (Servo): Botón Z + JoyY
            ang_muneca = constrain(ang_muneca + (delta*SENSIBILIDAD_MUNECA), 0, 180);
        } 
        else {
            // HOMBRO (Motor DC): Solo JoyY (sin botones)
            ang_hombro += (delta*SENSIBILIDAD_HOMBRO);
        }
    }

    // --- 3. CONVERSIÓN A SALIDAS REALES ---
    
    // Para los motores DC (Cintura y Hombro), convertimos grados a pasos
    // Reducción 450:1 -> 4950 pasos por cada 360 grados
    float pasos_cintura = ang_cintura * (2970.0 / 360.0);
    float pasos_hombro  = ang_hombro  * (2970.0 / 360.0);

    // Para los Servos (Codo y Muñeca), enviamos el ángulo directamente (0-180)
    float val_codo   = (float)ang_codo;
    float val_muneca = (float)ang_muneca;

    

    // Devolvemos el vector con las 4 posiciones
    // [0]=Cintura(pasos), [1]=Hombro(pasos), [2]=Codo(grados), [3]=Muñeca(grados)
    return {pasos_cintura, pasos_hombro, val_codo, val_muneca};
}

void nunchuck_debug_teleplot() {
    nunchuck.readData();
    // Formato estándar para Teleplot
    Serial.print(">JoyX:");
    Serial.println(nunchuck.getJoyX());
    
    Serial.print(">JoyY:");
    Serial.println(nunchuck.getJoyY());
    
    /*Serial.print(">BtnC:");
    Serial.println(nunchuck.getButtonC() ? 1 : 0);
    
    Serial.print(">BtnZ:");
    Serial.println(nunchuck.getButtonZ() ? 1 : 0);*/
}