const int in1Pin = 4;
const int in2Pin = 5;
const int enaPin = 6;
const int pinChannelA = 2;
const int pinChannelB = 3;
float lastSpeed = 0, motorSpeed = 0;
double count = 0;
float vel = 0;

// 1. Definir la frecuencia de interrupción deseada en Hz.
// (Registro de coincidencia) = [16,000,000 Hz / (preescalador * (frecuencia de interruptción deseada en Hz))] - 1

// Tabla de preescaladores para el Timer1

/*
----------------------------------------------------------------------------------
| CS12 | CS11 | CS10 | Prescaler        | Descripción                             |
|------|------|------|------------------|-----------------------------------------|
|  0   |  0   |  0   | 0 (No clock)     | No clock source (Timer/Counter stopped) |
|  0   |  0   |  1   | 1 (No prescaling)| clkI/O (No prescaling)                  |
|  0   |  1   |  0   | 8                | clkI/O / 8 (From prescaler)             |
|  0   |  1   |  1   | 64               | clkI/O / 64 (From prescaler)            |
|  1   |  0   |  0   | 256              | clkI/O / 256 (From prescaler)           |
|  1   |  0   |  1   | 1024             | clkI/O / 1024 (From prescaler)          |
|  1   |  1   |  0   | External (Falling)| External clock on T1 pin (falling edge)|
|  1   |  1   |  1   | External (Rising)| External clock on T1 pin (rising edge)  |
-----------------------------------------------------------------------------------
*/

// Ejemplos de configuración del preescalador para Timer1:

// No prescaling (clkI/O):
// TCCR1B |= (1 << CS10);

// Prescaler de 8 (clkI/O / 8):
// TCCR1B |= (1 << CS11);

// Prescaler de 64 (clkI/O / 64):
// TCCR1B |= (1 << CS11) | (1 << CS10);

// Prescaler de 256 (clkI/O / 256):
// TCCR1B |= (1 << CS12);

// Prescaler de 1024 (clkI/O / 1024):
// TCCR1B |= (1 << CS12) | (1 << CS10);

// External clock on T1 pin (falling edge):
// TCCR1B |= (1 << CS12) | (1 << CS11);

// External clock on T1 pin (rising edge):
// TCCR1B |= (1 << CS12) | (1 << CS11) | (1 << CS10);

void setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 62499;            // (Registro de coincidencia) = [16,000,000 Hz / (preescalador * (frecuencia de interruptción deseada en Hz))] - 1
  TCCR1A |= (1 << WGM12);
  TCCR1B |= (1 << CS12);    // Prescalador: 256
  TIMSK1 |= (1 << OCIE1A);
  sei();

  Serial.begin(9600);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(pinChannelA, INPUT);
  pinMode(pinChannelB, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinChannelA), callback_A, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinChannelB), callback_B, FALLING);
}

ISR(TIMER1_COMPA_vect) {
  vel = count / 22 / 45;
  count = 0;
}

void callback_A() {
  if (digitalRead(pinChannelB)) {
    count++;
  } else {
    count--;
  }
}

void callback_B() {
  if (!digitalRead(pinChannelA)) {
    count++;
  } else {
    count--;
  }
}

void loop() {

  // Revisar que si se estén ingresando nuevos valores en el monitor serial
  if (Serial.available() > 0) {
    // Leer la entrada del monitor serial cuando realmente se esté escribiendo un nuevo valor
    String input = Serial.readStringUntil('\n');

    // Convertir el valor leído a un número flotante
    float motorInput = input.toFloat();

    if (motorInput > 1.0) motorInput = 1.0;   // En caso de tener un valor mayor a 1
    if (motorInput < -1.0) motorInput = -1.0; // En caso de tener un valor menor a -1

    // Mapear el rango de -1.0 a 1.0 a -255 a 255
    motorSpeed = motorInput * 255.0;

    // Verificar si el valor para la velocidad ha cambiado, de no ser así mantener el mismo
    if (motorSpeed != lastSpeed) {
      lastSpeed = motorSpeed;
      Serial.println(vel);
    }

    // Dirección de giro de la cruz hacia la derecha en caso de ser un valor positivo (sentido anti-horario)
    if (lastSpeed > 0) {
      digitalWrite(in1Pin, HIGH);
      digitalWrite(in2Pin, LOW);
      analogWrite(enaPin, lastSpeed);
    } 
    // Dirección de giro hacia la izquierda en caso de ser un valor negativo (sentido horario)
    if (lastSpeed < 0) {
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, HIGH);
      analogWrite(enaPin, abs(lastSpeed));
    } 
    
    // Motor detenido en caso de ser un valor igual a 0
    if (lastSpeed == 0) {
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, LOW);
      analogWrite(enaPin, 0);
    }
  }
}
