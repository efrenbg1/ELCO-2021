#include <Arduino.h>

#define MOTOR 25
#define BOOST_CONV 26
#define ADC_HUMEDAD 27

#define HUMEDAD_100 2.35
#define HUMEDAD_0 3.3
#define HUMEDAD_LIMITE_INFERIOR 30
#define HUMEDAD_LIMITE_SUPERIOR 60

#define TIMEOUT_RIEGO 60 * 1000

#define VCC_IC 3.3

#define SLEEP_TIME 1 * 1000000

float medir_humedad()
{
    float humedad = analogRead(ADC_HUMEDAD) * VCC_IC / 4095.0;
    Serial.print("HUM=");
    Serial.print(humedad);
    Serial.print("V -> ");
    humedad = 100 * (humedad - HUMEDAD_0) / (HUMEDAD_100 - HUMEDAD_0);
    Serial.print(humedad);
    Serial.println("%");
    Serial.flush();
    return humedad;
}

void setup()
{
    Serial.begin(9600);

    pinMode(ADC_HUMEDAD, INPUT);
    pinMode(BOOST_CONV, OUTPUT);
    pinMode(MOTOR, OUTPUT);

    digitalWrite(BOOST_CONV, HIGH);
    digitalWrite(MOTOR, LOW);
    delay(200);
    float humedad = medir_humedad();

    if (humedad < HUMEDAD_LIMITE_INFERIOR)
    {
        Serial.println("Regando...");
        digitalWrite(MOTOR, HIGH);
        unsigned long t_inicio = millis();
        while (humedad < HUMEDAD_LIMITE_SUPERIOR)
        {
            delay(5000);
            humedad = medir_humedad();
            if ((millis() - t_inicio) > TIMEOUT_RIEGO){
                Serial.println("Se acabó el tiempo límite para regar");
                break;
            }
        }
    }

    digitalWrite(BOOST_CONV, LOW);
    digitalWrite(MOTOR, LOW);

    Serial.println("Durmiendo...");
    Serial.flush();

    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    esp_deep_sleep_start();
}

void loop()
{
}