#include <MandoNunchuck.h>

//Creamos objeto nunchuck
Accessory nunchuck;

//Guardamos los ángulos para que el robot se quede quieto al soltar el mando
double ang_cintura = 0.0;
double ang_hombro  = 0.0;
double ang_codo    = 90.0;
double ang_muneca  = 0.0;

//Robot
double d1 = 10.0; //Altura desde la base hasta el eje del hombro
double d2 = 12.0; //Longitud del brazo (hombro a codo)
double d3 = 12.0; //Longitud del antebrazo (codo a muñeca)
double d4 = 7.8;  //Distancia del eje de la muñeca a la punta
double h  = 6.5;  //Altura de la herramienta (punta del rotulador)
//double angulo_ataque = 45.0; //El rotulador apunta recto hacia abajo

//Mando
const int CENTRO_X = 120;
const int CENTRO_Y = 122;
const int DEADZONE = 10;    // Solo se mueve cuando el joystick está casi al límite

//Velocidad de cambio de ángulo
const float SENSIBILIDAD_MUNECA = 2.0; //76.9 rpm son 461.5 grados por segundo y entre 100 Hz (10 ms) son 4.61 grados por ciclo maximos. 
const float SENSIBILIDAD_CODO = 2.0 ; //76.9 rpm son 461.5 grados por segundo y entre 100 Hz (10 ms) son 4.61 grados por ciclo maximos.
const float SENSIBILIDAD_HOMBRO = 0.5; //18 rpm son 108 grados por segundo y entre 100 Hz (10 ms) son 1.08 grados por ciclo maximos.
const float SENSIBILIDAD_CINTURA = 1; // 37 rpm son 222 grados por segundo y entre 100 Hz (10 ms) son 2.22 grados por ciclo máximos. 

//Iman
bool iman_encendido = false;
bool botones_bloqueados = false; // Para evitar que el imán parpadee

//Modo de funcionamiento
bool modo_cartesiano = false; // false = Directa (Ángulos), true = Inversa (XYZ)

//Posición inicial segura
double robot_x = 15.5; // Distancia hacia adelante (Ponemos 1 para evitar singularidades)
double robot_y = 0.0;  // Distancia a un lado
double robot_z = 15.5; // Altura inicial

const double angulo_lienzo = 45.0 * (PI / 180.0); // 45 grados a radianes

// Variables para suavizado (añádelas arriba)
float pasos_cintura_suaves = 0;
float pasos_hombro_suaves = 0;
const float ALFA = 0.05; // Factor de suavizado (0.1 a 0.3 es ideal)


void nunchuck_init() {
    nunchuck.begin();
}

void manejar_nunchuck_y_cinematica(float &pasos_cintura, float &pasos_hombro, float &val_codo, float &val_muneca) {

    //Leemos datos del nunchuck
    nunchuck.readData();

    // =======================================================
    // --- INICIO DEL TEST ESTRICTO DE CINEMÁTICA INVERSA ---
    // =======================================================

    // 1. Definimos el punto HOME (Punto de confort)
    double test_x = 21.0; 
    double test_y = 0.0;  // Centro
    double test_z = 15.0; // Altura

    // 2. Leemos los botones para cambiar de punto
    if (nunchuck.getButtonZ()) {
        // PUNTO A: 5 cm a la derecha (Solo cambia la Y)
        test_y = 5.0;
    } 
    else if (nunchuck.getButtonC()) {
        // PUNTO B: 5 cm hacia adelante (Solo cambia la X)
        test_x = 26.0;
    }

    // 3. Calculamos la cinemática
    double t1, t2, t3, t4;
    bool ok = buscar_postura_variable(test_x, test_y, test_z, d1, d2, d3, d4, h, t1, t2, t3, t4);

    if (ok) {
        ang_cintura = t1;
        ang_hombro  = t2; // Ajusta este +90 según tu montaje real
        ang_codo    = t3;        // OJO: Pon +90.0 aquí solo si descubriste que lo necesitas
        ang_muneca  = t4;
    }

    // 4. Traducción directa a pasos (con el ratio CORREGIDO del hombro)
    float pasos_cintura_target = ang_cintura * (2970.0 / 360.0);
    float pasos_hombro_target  = ang_hombro  * (2970.0 / 360.0);

    // Para este test, nos saltamos el filtro ALFA para ver el movimiento puro
    pasos_cintura = pasos_cintura_target;
    pasos_hombro  = pasos_hombro_target;

    val_codo   = (float)ang_codo;
    val_muneca = (float)ang_muneca;

    // Salimos de la función aquí para que el joystick no interfiera
    return; 
    
    // =======================================================
    // --- FIN DEL TEST ---
    // =======================================================

    //Añadir boton para cambiar entre modo cinemática directa o inversa para usar el nunchuck para pintar

    /*if (modo_cartesiano) //Modo papel
    {
        double vel = 0.2; // mm por ciclo (Ajusta la velocidad de dibujo aquí)
        double delta_p_x = 0, delta_p_y = 0, delta_p_z = 0; //Diferencias de posición para el rotu

        //JoyX controla el eje Y del robot (Izquierda/Derecha)
        if (abs(nunchuck.getJoyX() - CENTRO_X) > DEADZONE)
        {
            delta_p_y = -((nunchuck.getJoyX() - CENTRO_X) / 98.5) * vel;
        } 
            
        //JoyY controla el avance sobre el papel inclinado
        if (abs(nunchuck.getJoyY() - CENTRO_Y) > DEADZONE)
        {
            delta_p_x = ((nunchuck.getJoyY() - CENTRO_Y) / 97.0) * vel;
        }

        // Botones controlan entrar/salir del papel (Presión)
        if (nunchuck.getButtonZ())
        {
            delta_p_z = vel;  // Acercar
        }
        if (nunchuck.getButtonC())
        {
            delta_p_z = -vel; // Alejar
        } 

        //Aplicamos cambio (Papel -> Robot)
        // El eje Y del robot es igual al eje X del papel
        double dx = delta_p_x * cos(angulo_lienzo) - delta_p_z * sin(angulo_lienzo);
        double dy = delta_p_y;
        double dz = delta_p_x * sin(angulo_lienzo) + delta_p_z * cos(angulo_lienzo);

        //Nueva posición según las desviaciones
        double nx = robot_x + dx;
        double ny = robot_y + dy;
        double nz = robot_z + dz;

        //Angulos de cada articulacion (Locales por seguridad)
        double t1, t2, t3, t4;

        // LLAMADA A TU CINEMÁTICA
        bool exito = buscar_postura_variable(nx, ny, nz, d1, d2, d3, d4, h, t1, t2, t3, t4);

        if (exito) //Si tenemos exito en el caluclo entonces actualizamos angulos y posiciones del robot
        {
            // ¡TODO OK! Actualizamos la posición real y guardamos como última válida
            robot_x = nx; 
            robot_y = ny; 
            robot_z = nz;

            ang_cintura = t1;
            ang_hombro  = t2+90.0;
            ang_codo    = t3+90.0;
            ang_muneca  = t4;
        
        }
        else
        {
            // Aunque no llegue el brazo, la base SÍ puede girar
            ang_cintura = atan2(robot_y, robot_x) * (180.0 / PI);
        }


    }

    else //Modo libre
    {
        //Movimiento de la cintura
        if (abs(nunchuck.getJoyX() - CENTRO_X) > DEADZONE) 
        {
            double delta = ((nunchuck.getJoyX() - CENTRO_X) / 98.5) * SENSIBILIDAD_CINTURA;
            ang_cintura = constrain(ang_cintura - (delta), -90, 90); //Limitamos movimiento del motor dc
            //ang_cintura += delta;
        }

        //Movimiento de hombro, codo y muñeca junto con iman
        if (nunchuck.getButtonC() && nunchuck.getButtonZ()) //Activamos imán
        {
            if (!botones_bloqueados)  //Bloqueo para que no esté encendiéndose y apagándose el imán
            {
                iman_encendido = !iman_encendido; 
                botones_bloqueados = true;   

                if (iman_encendido)
                {
                    activar_iman(pin_iman);
                }
                else 
                {
                    desactivar_iman(pin_iman);
                }
            }
        } 
        else 
        {
            // Si no están los dos pulsados, liberamos el bloqueo para la próxima pulsación, para poder desactivarlo si ya lo habíamos activado
            botones_bloqueados = false; 

            //Si hay movimiento del Joystick:
            if (abs(nunchuck.getJoyY() - CENTRO_Y) > DEADZONE) 
            {
                double delta = ((nunchuck.getJoyY() - CENTRO_Y) / 97.0);
                
                if (nunchuck.getButtonC()) //Movimiento Codo
                {
                    ang_codo = constrain(ang_codo - (delta*SENSIBILIDAD_CODO), 0, 180); //Limitamos movimiento del servo
                    //ang_codo -= (delta*SENSIBILIDAD_CODO);
                } 
                else if (nunchuck.getButtonZ()) //Movimiento muñeca
                {
                    ang_muneca = constrain(ang_muneca + (delta*SENSIBILIDAD_MUNECA), -90, 130); //Limitamos movimiento del servo
                    //ang_muneca += (delta*SENSIBILIDAD_MUNECA);
                } 
                else //Movmiento hombro
                {
                    ang_hombro = constrain(ang_hombro + (delta*SENSIBILIDAD_HOMBRO), -90, 90); //Limitamos movimiento del motor dc
                    //ang_hombro += (delta*SENSIBILIDAD_HOMBRO);
                }
            }
        }
        
    }

    // Para los motores DC (Cintura y Hombro), convertimos grados a pasos
         pasos_cintura = ang_cintura * (2970.0 / 360.0);
         pasos_hombro  = ang_hombro  * (2970.0 / 360.0);

        // Para los Servos (Codo y Muñeca), enviamos el ángulo directamente (0-180)
         val_codo   = (float)ang_codo;
         val_muneca = (float)ang_muneca;

        float pasos_cintura_target = ang_cintura * (2970.0 / 360.0);
        float pasos_hombro_target  = ang_hombro  * (2970.0 / 360.0);

        // FILTRO: Nueva_posicion = vieja + 20% del cambio
        //pasos_cintura_suaves = pasos_cintura_suaves + ALFA * (pasos_cintura_target - pasos_cintura_suaves);
        //pasos_hombro_suaves  = pasos_hombro_suaves + ALFA * (pasos_hombro_target - pasos_hombro_suaves);

        pasos_cintura = pasos_cintura_target;
        pasos_hombro  = pasos_hombro_target;

        val_codo      = (float)ang_codo;
        val_muneca    = (float)ang_muneca;*/
}

void nunchuck_debug_teleplot() {
    // Formato estándar para Teleplot
    Serial.print(">JoyX:");
    Serial.println(nunchuck.getJoyX());
    
    Serial.print(">JoyY:");
    Serial.println(nunchuck.getJoyY());
    
    Serial.print(">BtnC:");
    Serial.println(nunchuck.getButtonC() ? 1 : 0);
    
    Serial.print(">BtnZ:");
    Serial.println(nunchuck.getButtonZ() ? 1 : 0);
}