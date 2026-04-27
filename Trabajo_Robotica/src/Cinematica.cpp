#include <Cinematica.h>

// Añade esta variable al principio de Cinematica.cpp (fuera de las funciones)
double ultimo_phi_exitoso = -45.0;

bool calcular_cinematica_inversa(double X, double Y, double Z, double phi_deg, 
                                 double d1, double d2, double d3, double d4, double h, 
                                 double &t1_deg, double &t2_deg, double &t3_deg, double &t4_deg) {
    
    //Convertimos el ángulo de ataque a radianes (Angulo de la herramienta)
    double phi = phi_deg * (PI / 180.0);
    
    //Giramos la base hasta el punto donde queremos ir (Calculamos angulo de la base)
    double t1 = atan2(Y, X);
    
    //Calculamos distancia desde centro de la base hasta punto objetivo
    double R = sqrt(X*X + Y*Y);
    
    //Adaptamos el eje Z invertido (positivo hacia abajo) para el rotulador
    double Z_plan = d1 - Z;
    
    //Desacople de la muñeca con d4 y h:
    //Calculamos la posición donde debe estar la articulación de la muñeca para que la punta del rotu toque el punto final
    double rw = R - d4 * cos(phi) + h * sin(phi); //Horizontal
    double zw = Z_plan - d4 * sin(phi) - h * cos(phi); //Vertical
    
    //Caculamos distnacia al punto final
    double D = (rw*rw + zw*zw - d2*d2 - d3*d3) / (2.0 * d2 * d3);
    
    // Comprobamos si el brazo llega
    if (D > 1.0 || D < -1.0) {
        Serial.print("ERROR: Punto inalcanzable. D = ");
        Serial.println(D);
        return false; // Punto inalcanzable según la ley del coseno
    }
    
    //Calculamos ángulo del codo
    double t3 = atan2(sqrt(1.0 - D*D), D);
    
    //Calculamos angulo del hombro
    double t2 = atan2(zw, rw) - atan2(d3 * sin(t3), d2 + d3 * cos(t3));
    
    //Calculamos angulo de la muñeca
    double t4 = phi - (t2 + t3);
    
    //Convertimos todo a grados
    t1_deg = t1 * (180.0 / PI);
    t2_deg = t2 * (180.0 / PI);
    t3_deg = t3 * (180.0 / PI);
    t4_deg = t4 * (180.0 / PI);

    Serial.print("Intentando postura -> ");
    Serial.print("Cintura(t1): "); Serial.print(t1_deg);
    Serial.print(" | Hombro(t2): "); Serial.print(t2_deg);
    Serial.print(" | Codo(t3): "); Serial.print(t3_deg);
    Serial.print(" | Muneca(t4): "); Serial.println(t4_deg);

    // 1. Definimos el límite del codo según la posición del hombro
    float limite_codo_max;

    if (t2_deg < -90.0) { 
        // Si el hombro está "hacia atrás" (más de 90° hacia arriba/atrás)
        limite_codo_max = 160.0; 
    } else {
        // Si el hombro está hacia adelante (cerca de la horizontal 0)
        limite_codo_max = 110.0; 
    }

    //Límites de grados para cada articulación
    if (t1_deg < -90 || t1_deg > 90)
    {
        Serial.print("ERROR: t1_deg = ");
        Serial.println(t1_deg);
        return false; // Cintura bloqueada
    }
    if (t2_deg < -180   || t2_deg > 0)
    {
        Serial.print("ERROR: t2_deg = ");
        Serial.println(t2_deg);
        return false; // Hombro no puede bajar del suelo
    } 
    if (t3_deg < -90 || t3_deg > limite_codo_max)
    {
        Serial.print("ERROR t3 (Límite: "); 
        Serial.print(limite_codo_max); 
        Serial.print("): "); 
        Serial.println(t3_deg);
        return false; // Codo no puede doblarse al revés
    }   
    if (t4_deg < -90 || t4_deg > 130)
    {
        Serial.print("ERROR: t4_deg = ");
        Serial.println(t4_deg);
        return false; // Muñeca no puede doblarse al revés
    }   
    
    return true; 
}

// Esta función intenta encontrar una postura válida variando el ángulo de ataque
bool buscar_postura_variable(double X, double Y, double Z, 
                             double d1, double d2, double d3, double d4, double h, 
                             double &t1_deg, double &t2_deg, double &t3_deg, double &t4_deg) {
    
    // PRIMERO: Intentamos con el mismo ángulo que funcionó la última vez
    /*if (calcular_cinematica_inversa(X, Y, Z, ultimo_phi_exitoso, d1, d2, d3, d4, h, 
                                    t1_deg, t2_deg, t3_deg, t4_deg)) {
        return true; 
    }

    // SEGUNDO: Si el anterior ya no sirve, buscamos uno nuevo de 1 en 1 grado
    // Usamos 1 grado en lugar de 5 para que los saltos sean más suaves
    for (double phi_test = -45.0; phi_test >= -90.0; phi_test -= 1.0) {
        if (calcular_cinematica_inversa(X, Y, Z, phi_test, d1, d2, d3, d4, h, 
                                        t1_deg, t2_deg, t3_deg, t4_deg)) {
            ultimo_phi_exitoso = phi_test; // Guardamos para la próxima
            return true;
        }
    }

    // Ángulo FIJO ideal para pintar (perpendicular al papel).
    // Ya no dejamos que el robot invente posturas raras.
    double phi_fijo = -45.0; 

    // Solo probamos la postura perfecta. 
    if (calcular_cinematica_inversa(X, Y, Z, phi_fijo, d1, d2, d3, d4, h, 
                                    t1_deg, t2_deg, t3_deg, t4_deg)) {
        return true; // Ha llegado limpiamente
    }

    // Si llega aquí, es que el punto es físicamente imposible con tus límites.
    // Devolvemos false y el robot se quedará congelado en la última posición válida (El muro de cristal).

    return false;*/

    // 1. CERO REBOTES: Intentamos la misma postura exacta que funcionó en el ciclo anterior.
    if (calcular_cinematica_inversa(X, Y, Z, ultimo_phi_exitoso, d1, d2, d3, d4, h, 
                                    t1_deg, t2_deg, t3_deg, t4_deg)) {
        return true; 
    }

    // 2. ADAPTACIÓN SUAVE: Si la postura anterior choca con un límite, 
    // buscamos un ángulo nuevo abriendo en abanico (1º menos, 1º más, 2º menos...).
    // Esto hace que la muñeca se flexione suavemente sin dar tirones bruscos.
    for (double offset = 1.0; offset <= 45.0; offset += 1.0) {
        
        // Probamos inclinando un poco hacia atrás
        double phi_test_1 = ultimo_phi_exitoso - offset;
        
        // Respetamos tu límite físico para no romper el servo
        if (phi_test_1 >= -90.0) {
            if (calcular_cinematica_inversa(X, Y, Z, phi_test_1, d1, d2, d3, d4, h, 
                                            t1_deg, t2_deg, t3_deg, t4_deg)) {
                ultimo_phi_exitoso = phi_test_1;
                return true;
            }
        }

        // Probamos inclinando un poco hacia adelante (hasta paralelo al suelo 0.0)
        double phi_test_2 = ultimo_phi_exitoso + offset;
        
        if (phi_test_2 <= 0.0) {
            if (calcular_cinematica_inversa(X, Y, Z, phi_test_2, d1, d2, d3, d4, h, 
                                            t1_deg, t2_deg, t3_deg, t4_deg)) {
                ultimo_phi_exitoso = phi_test_2;
                return true;
            }
        }
    }

    // Si probando todo no llega, es el límite absoluto de la física.
    return false;
}

void calcular_cinematica_directa(double t1_deg, double t2_deg, double t3_deg, double t4_deg,
                                 double d1, double d2, double d3, double d4, double h,
                                 double &X, double &Y, double &Z) {
    // Pasar a radianes
    double t1 = t1_deg * (PI / 180.0);
    double t2 = t2_deg * (PI / 180.0);
    double t3 = t3_deg * (PI / 180.0);
    double t4 = t4_deg * (PI / 180.0);

    // Ángulo global de la muñeca (phi)
    double phi = t2 + t3 + t4;

    // Radio horizontal total (R)
    double R = d2 * cos(t2) + d3 * cos(t2 + t3) + d4 * cos(phi) - h * sin(phi);
    
    // Coordenadas finales
    X = R * cos(t1);
    Y = R * sin(t1);
    
    // Altura (Z) - Recordando que Z_plan = d1 - Z
    double Z_plan = d2 * sin(t2) + d3 * sin(t2 + t3) + d4 * sin(phi) + h * cos(phi);
    Z = d1 - Z_plan;
}