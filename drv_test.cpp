#include <Arduino.h>

void setup()
{
  Serial.begin(9600);

  pinMode(16, OUTPUT);
  analogWrite(16, 0);

  delay(200);

  Serial.println("Escriba el valor de la frecuencia PWM...");
}

void loop()
{
  if (Serial.available() > 0)
  {
    String frecuencia = Serial.readStringUntil('\n');
    int target = frecuencia.toInt();
    if (target == 0)
    {
      Serial.println("Uso: escriba la frecuencia PWM");
    }
    analogWriteFreq(target);
    analogWrite(16, 127);
  }
}
