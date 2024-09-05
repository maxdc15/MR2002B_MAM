const int in1Pin = 4;
const int in2Pin = 5;
const int enaPin = 6;
const int STANDBY = 7;
const int pinChannelA = 2;
const int pinChannelB = 3;
double lastSpeed = 0, motorSpeed = 0;
volatile int count = 0;
double vel = 0;
double pwm = 0, command, speed = 0;
const int win_size = 10;
double avg = 0, sum = 0;
int index = 0;
double readings [win_size];


/*
Tabla de preescaladores para el Timer1
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

  Serial.begin(2000000);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(STANDBY, OUTPUT);
  digitalWrite(STANDBY, HIGH);
  pinMode(pinChannelA, INPUT);
  pinMode(pinChannelB, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinChannelA), callback_A, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinChannelB), callback_B, FALLING);
}

void callback_A() { if (digitalRead(pinChannelB)) {count++;} else {count--;} }
void callback_B() { if (!digitalRead(pinChannelA)) {count++;} else {count--;} }

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    command = input.toFloat();
    moveMotor(command);
  }
  Serial.println(counts);
}

void moveMotor(double data) {
  pwm = max(-1.0, min(data, 1.0));
  if (pwm >= 0) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  } else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  }
  analogWrite(enaPin, int(255*abs(pwm)));
}
