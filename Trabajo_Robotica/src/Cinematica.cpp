#include <Cinematica.h>

/*
double L1, L2;
//https://docs.arduino.cc/language-reference/#functions
int calculo_angulos(double x, double y, double z) //Pasamos lel punto donde queremos ir
{
    //PLANO X-Y (El del papel)

    double hipotenusa1 = pow(x,2)+pow(y,2);

    if(hipotenusa1>(L1+L2))
    {
        Serial.print("IMPOSIBLE LLEGAR");
        return 1;
    }

    else//Angulo de giro para posicionarse en frente del objeto o punto
    {
        double alfa=atan2(y,x)*(180.0/PI);
    }
    



    //PLANO X-Z (El de la pared)

    double hipotenusa2 = pow(x,2)+pow(z,2);

    if(hipotenusa2>(L1+L2))
    {
        Serial.print("IMPOSIBLE LLEGAR");
        return 1;
    }

    else
    {
        //Angulo brazo 1 rad
        // Formula: cos(theta2) = (X^2 + Y^2 - L1^2 - L2^2) / (2 * L1 * L2)
        double num = hipotenusa2-(pow(L1,2))-(pow(L2,2));
        double den = 2*L1*L2;
        double theta2_rad = acos(num/den);

        //Angulo brazo 2 rad
        // Formula: atan2(Y, X) - atan2(L2 * sin(theta2), L1 + L2 * cos(theta2))
        double prim = L1+L2*cos(theta2_rad);
        double seg = L2*sin(theta2_rad);
        double theta1_rad = atan2(z, x)-atan2(prim, seg);

        //Angulo brazo 1 y 2 en grados
        double theta1_grad = theta1_rad*(180.0/PI);
        double theta2_grad = theta2_rad*(180.0/PI);
    }
    

}
*/

bool calcular_cinematica_inversa(double X, double Y, double Z, double phi_deg, 
                                 double d1, double d2, double d3, double d4, double h, 
                                 double &t1_deg, double &t2_deg, double &t3_deg, double &t4_deg) {
    
    // 1. Convertimos el ángulo deseado a radianes
    double phi = phi_deg * (M_PI / 180.0);
    
    // 2. Base (t1)
    double t1 = atan2(Y, X);
    
    // 3. Radio horizontal
    double R = sqrt(X*X + Y*Y);
    
    // 4. Adaptamos el eje Z invertido (positivo hacia abajo)
    double Z_plan = d1 - Z;
    
    // 5. EL NUEVO DESACOPLE DE LA MUÑECA (Con d4 y h)
    // Como la Z del rotulador apunta hacia ABAJO (+h):
    // - En horizontal (rw): retrocedemos d4, pero sumamos el efecto de h si se inclina.
    // - En vertical (zw): restamos d4 si se inclina, y restamos h (porque la muñeca está más alta que la punta).
    double rw = R - d4 * cos(phi) + h * sin(phi);
    double zw = Z_plan - d4 * sin(phi) - h * cos(phi);
    
    // 6. Codo (t3)
    double D = (rw*rw + zw*zw - d2*d2 - d3*d3) / (2.0 * d2 * d3);
    
    // Comprobamos si el brazo llega
    if (D > 1.0 || D < -1.0) {
        return false; // Punto inalcanzable
    }
    
    double t3 = atan2(-sqrt(1.0 - D*D), D);
    
    // 7. Hombro (t2)
    double t2 = atan2(zw, rw) - atan2(d3 * sin(t3), d2 + d3 * cos(t3));
    
    // 8. Muñeca (t4)
    double t4 = phi - (t2 + t3);
    
    // 9. Convertimos todo a grados
    t1_deg = t1 * (180.0 / M_PI);
    t2_deg = t2 * (180.0 / M_PI);
    t3_deg = t3 * (180.0 / M_PI);
    t4_deg = t4 * (180.0 / M_PI);
    
    return true; 
}