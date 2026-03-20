#include <Arduino.h>
#include <Encoder.h>
#include <Driver.h>
#include <PID.h>
#include <Wire.h>//I2C
#include <WiiChuck.h>


// put function declarations here:

Accessory nunchuck;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); // Para ver los pasos en el monitor
  encoder_init();
  driver_init();

  nunchuck.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  nunchuck.readData();

  int lecturaX = nunchuck.getJoyX();

  // Mapeamos el joystick a un "paso" de movimiento (-5 a 5)
  int paso = map(lecturaX, 35, 215, -5, 5);

  // Si el joystick no está en el centro (zona muerta entre 120 y 135)
  if (lecturaX < 120 || lecturaX > 135) {
      pos_objetivo = pos_objetivo + paso; 
  }

  // Evitamos que el objetivo se salga de los límites físicos (0 a 1200)
  pos_objetivo = constrain(pos_objetivo, 0, 1200);


  print_encoder();
  print_PID();

  float u=control_pos(pos_encoder);

  int dir=1;//Dirección de la señal de control
  if(u<0)
  {
    dir=-1;
  }
  
  float pwr=fabs(u);//Potencia de la señal de control
  if(pwr>255)//Máximo de potencia
  {
    pwr=255;
  }

  if(pwr>0 && pwr < 50) //Minimo de potencia ya que sino no se mueve por fricción (Hay que ir probando experimentalmente)
  {
    pwr = 50;
  }

  manejo_driver(dir, pwr, pin_ENA, pin_IN1, pin_IN2);


}
