/* 
  Ejemplo de control de un motor DC usando modulo L298
 
  Este programa permite controlar un motor de corriente directa mediante un puente H,
  utilizando un valor ingresado por el usuario desde el Monitor Serial para ajustar la velocidad y
  dirección del motor. El motor continuará girando de forma indefinida con el último valor ingresado
  hasta que se reciba un nuevo valor en el Monitor Serial. Los valores aceptados 
  van desde -255 (giro inverso a máxima velocidad) hasta 255 (giro hacia adelante
  a máxima velocidad), con 0 deteniendo el motor.
 
  Creado 19/Agosto/2024
  por Maximiliano De La Cruz Lima y Eduardo Chavez Martín
 */

const int in1Pin = 4;
const int in2Pin = 5;
const int enaPin = 6;
int lastSpeed = 0, motorSpeed = 0;

void setup() {
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  // Revisar que si se estén ingresando nuevos valores en el monitor serial
  if (Serial.available() > 0) {
    
    // Leer la entrada del monitor serial cuando realmente se esté escribiendo un nuevo valor
    String input = Serial.readStringUntil('\n');
    
    // Convertir el valor leído como una cadena de caracteres a un valor numérico
    motorSpeed = input.toInt();

    // Verificar si el valor para la velocidad ha cambiado, de no ser así mantener el mismo
    if(motorSpeed != lastSpeed){
          lastSpeed = motorSpeed;
        }

    // Mantener los valores dentro del rango de representación de 8 bits ([255, -255])
    if (lastSpeed > 255) lastSpeed = 255;   // En caso de tener un valor mayor a 255
    if (lastSpeed < -255) lastSpeed = -255; // En caso de tener un valor menor a -255

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
      analogWrite(enaPin, -lastSpeed);
    } 
    
    // Motor detenido en caso de ser un valor igual a 0
    if (lastSpeed == 0) {
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, LOW);
      analogWrite(enaPin, 0);
    }

  }
}
