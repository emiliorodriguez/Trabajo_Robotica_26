# 🤖 Brazo Robótico de 4 DOF con Control Cinemático y Autónomo

Este proyecto consiste en el diseño, programación y control de un brazo robótico de 4 grados de libertad (DOF) desarrollado para la asignatura de **Robótica**. El sistema combina control manual mediante un mando Wii Nunchuck y un modo autónomo capaz de realizar trayectorias precisas utilizando cinemática inversa y control PID de lazo cerrado.

## 👥 Equipo de Trabajo
* [Arnaiz, David]
* [Delgado, Raúl]
* [Frenzel, Daniel]
* [Rodríguez, Emilio]
* [Zurera, Fernando]

---

## 🚀 Características Principales

- **Control Híbrido:** - **Manual:** Movimiento cartesiano (X, Y, Z) en el plano del lienzo mediante joystick.
  - **Autónomo:** Máquina de estados para ejecución de trayectorias preprogramadas.
- **Cerebro:** Arduino Mega 2560 (aprovechando sus múltiples interrupciones de hardware).
- **Control de Movimiento:**
  - Control de posición mediante **PID** para motores DC con encoders.
  - Control directo de posición para Servomotores.
- **Matemáticas:** Implementación de **Cinemática Inversa** para desacoplamiento de muñeca.
- **Actuador Final:** Electroimán controlado por MOSFET para manipulación de objetos metálicos.

---

## 🛠️ Hardware Utilizado

| Componente | Cantidad | Descripción |
| :--- | :---: | :--- |
| **Arduino Mega 2560** | 1 | Microcontrolador principal. |
| **Motores JGB37-520** | 2 | Motores DC con reductora y encoders (Cintura y Hombro). |
| **Servos MG996R** | 2 | Servos de alto torque (Codo y Muñeca). |
| **L298N** | 1 | Puente H doble para control de motores DC. |
| **Wii Nunchuck** | 1 | Mando para control manual por I2C. |
| **Electroimán** | 1 | Actuador para transporte de piezas. |
| **IRLZ44N MOSFET** | 1 | Control de potencia del electroimán con protección por diodo flyback. |
| **Fuente 12V 5A** | 1 | Alimentación de potencia. |

---

## 📊 Mapeo de Pines (Arduino Mega)

Para garantizar la precisión, se han utilizado los pines de interrupción nativos:

- **Motores DC:**
  - Motor A (Cintura): ENA(6), IN1(7), IN2(8) | Encoders: 18, 19.
  - Motor B (Hombro): ENB(11), IN3(9), IN4(10) | Encoders: 2, 3.
- **I2C (Nunchuck):** SDA(20), SCL(21).
- **Servos:** Servo 1(12), Servo 2(13).
- **Electroimán:** Pin 5 (vía MOSFET).

---

## 💻 Estructura del Software

El código se ha desarrollado de forma modular para facilitar el mantenimiento y la escalabilidad:

- `/src/Cinematica`: Cálculo de ángulos basados en coordenadas cartesianas y geometría del brazo.
- `/src/ControlHardware`: Capa de abstracción que unifica el movimiento de motores, servos e imán.
- `/src/PID`: Algoritmos de control de lazo cerrado con constantes `Kp`, `Ki` y `Kd`.
- `/src/Trayectoria`: Generador de micropuntos para movimientos rectilíneos suaves.
- `/src/Estados`: Máquina de estados finitos (FSM) para la lógica de dibujo autónomo.

---

## ⚙️ Instalación y Configuración

1. **Requisitos:** Tener instalado [VS Code](https://code.visualstudio.com/) y la extensión [PlatformIO](https://platformio.org/).
2. **Clonar repositorio:**
   ```bash
   git clone [https://github.com/emiliorodriguez/Trabajo_Robotica_26](https://github.com/emiliorodriguez/Trabajo_Robotica_26)
