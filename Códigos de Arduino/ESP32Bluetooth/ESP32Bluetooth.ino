#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Test"); // Nombre del dispositivo Bluetooth
  Serial.println("El dispositivo Bluetooth está listo para emparejarse");
}

void loop() {
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    Serial.print("Recibido: ");
    Serial.println(incomingChar);
  }
}
