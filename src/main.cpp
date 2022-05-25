#include <Arduino.h>

// Definición de conexiones
#define MOTOR 25
#define BOOST_CONV 26
#define ADC_HUMEDAD 27

// Calibración de sensor de humedad
#define HUMEDAD_100 2.35
#define HUMEDAD_0 3.3
#define VCC_IC 3.3

// Límite de humedad
#define HUMEDAD_LIMITE_INFERIOR 30

// Constantes temporales
#define TIMEOUT_RIEGO 10 * 1000
#define SLEEP_TIME 2 * 60 * 1000000

// Leer humedad y convertir a humedad relativa
float medir_humedad()
{
    // Leer valor en voltaje
    float humedad = analogRead(ADC_HUMEDAD) * VCC_IC / 4095.0;

    // Imprimir valor en voltios
    Serial.print("HUM=");
    Serial.print(humedad);
    Serial.print("V -> ");

    // Convertir a humedad relativa
    humedad = 100 * (humedad - HUMEDAD_0) / (HUMEDAD_100 - HUMEDAD_0);

    // Imprimir también la humedad relativa
    Serial.print(humedad);
    Serial.println("%");
    Serial.flush();

    return humedad;
}

// Obtener si el botón ha sido la causa por la que se ha despertado del deep sleep
bool wakeup_boton()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        return true;
        break;
    default:
        return false;
        break;
    }
}

void setup()
{
    Serial.begin(9600);

    // Asignar E/S
    pinMode(ADC_HUMEDAD, INPUT);
    pinMode(BOOST_CONV, OUTPUT);
    pinMode(MOTOR, OUTPUT);

    // Encender Boost converter pero no el motor
    digitalWrite(BOOST_CONV, HIGH);
    digitalWrite(MOTOR, LOW);

    // Esperar a que el sensor se estabilice
    delay(200);

    float humedad = medir_humedad();

    // Regar si se ha pulsado el botón o la humedad está baja
    if (wakeup_boton() || humedad < HUMEDAD_LIMITE_INFERIOR)
    {
        Serial.println("Regando...");
        digitalWrite(MOTOR, HIGH);
        unsigned long t_inicio = millis();
        while (1)
        {
            delay(5000);
            medir_humedad();
            if ((millis() - t_inicio) > TIMEOUT_RIEGO)
            {
                Serial.println("Se acabó el tiempo límite para regar");
                break;
            }
        }
    }

    // Apagar todo
    digitalWrite(BOOST_CONV, LOW);
    digitalWrite(MOTOR, LOW);

    Serial.println("Durmiendo...");
    Serial.flush();

    // Activar wake por timeout o botón
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);

    // Entrar en deep sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Al despertarse de deep sleep se produce un reset. Por lo que nunca se llega aquí
}