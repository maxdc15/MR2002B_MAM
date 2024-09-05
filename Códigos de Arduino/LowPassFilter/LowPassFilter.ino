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

void setup() {
  cli();
  TCCR1A = 0;               // Configuración del registro de comparación
  TCCR1B = 0;  
  TCNT1  = 0;  
  OCR1A = 2499;             // Para 25 Hz (prescaler de 256)
  TCCR1B |= (1 << WGM12);   // Modo de comparación de salida (CTC - Clear Timer on Compare Match)
  TCCR1B |= (1 << CS12);    // Prescaler de 256
  TIMSK1 |= (1 << OCIE1A);  // Habilita la interrupción por comparación de Timer1
  sei();

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

ISR(TIMER1_COMPA_vect) {
  speed = float(count)*(25.0/22.0)*(1.0/45.0);
  count = 0;

  sum = sum - readings[index];
  readings[index] = speed;
  sum = sum + speed;
  avg = sum/win_size;
  index = (++index) % win_size;
}

void callback_A() { if (digitalRead(pinChannelB)) {count++;} else {count--;} }
void callback_B() { if (!digitalRead(pinChannelA)) {count++;} else {count--;} }

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    command = input.toFloat();
    moveMotor(command);
    Serial.println(avg);
  }
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
