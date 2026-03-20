#include <Cinematica.h>

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