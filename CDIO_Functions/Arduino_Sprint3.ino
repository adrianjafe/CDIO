#include "libreria.h"

const int power_pin = 5; // Replace with the actual pin number

Sensores sensores(power_pin);

void connectWiFi() {
    // Your WiFi connection code here
}

void setup() {
    Serial.begin(9600);
    connectWiFi();
    pinMode(power_pin, OUTPUT);
     
}

void loop() {
    const unsigned int canalTemperatura = 1; // Replace with the actual channel number
    const unsigned int canalHumedad = 2;      // Replace with the actual channel number
    const unsigned int canalPH = 0;           // Replace with the actual channel number
    const unsigned int canalluz = 3;          // Replace with the actual channel number

    float temperatura = sensores.leerTemperatura(canalTemperatura);
    float humedad = sensores.leerHumedad(canalHumedad);
    int salinidad = sensores.leerSalinidad();
    int pH = sensores.leerPH(canalPH);
    float luzVoltage;
    int16_t luz = sensores.leerLuz(canalluz, luzVoltage);

    Serial.print("Temperatura: ");
    Serial.println(temperatura);
    Serial.print("Humedad: ");
    Serial.println(humedad);
    Serial.print("Salinidad :");
    Serial.println(salinidad);
    Serial.print("PH: ");
    Serial.println(pH);
    Serial.print("Luz: ");
    Serial.println(luz);
    // Realiza las operaciones necesarias con los datos leídos
    delay(1000);
}
