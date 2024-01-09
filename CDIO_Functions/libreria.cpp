/// sensores.cpp

#include "libreria.h"

Sensores::Sensores(int pin) : powerPin(pin) {
    ads1115.begin(0x48);
    ads1115.setGain(GAIN_ONE);

}

float Sensores::leerTemperatura(unsigned int canal) {
    int16_t adc = ads1115.readADC_SingleEnded(canal);
    float voltage = (adc * 4.096) / 32767;
    float temperatura = (voltage - 0.79) / 0.033 + 0.5; // Ajusta la f�rmula seg�n tus necesidades
    return temperatura;
}

float Sensores::leerHumedad(unsigned int canal) {
    int16_t adc = ads1115.readADC_SingleEnded(canal);
    int humedad = map(adc, 17000, 2300, 0, 100);
    if (humedad > 100) {
        humedad = 100;
    }
    return humedad;
}

int Sensores::leerSalinidad() {
    digitalWrite(powerPin, HIGH);
    delay(100);
    int16_t adc = analogRead(A0); // Canal de salinidad
    digitalWrite(powerPin, LOW);
    delay(100);
    int salinidad = map(adc, 430, 900, 0, 100);
    if (salinidad > 100) {
        salinidad = 100;
    }
    if (salinidad < 0) {
        salinidad = 0;
    }
    return salinidad;
}

int Sensores::leerPH(unsigned int canal) {
    int16_t adc = ads1115.readADC_SingleEnded(canal);
    float voltage = (adc * 4.096) / (pow(2 , 15)-1);
    float pHValue = 3.5 * voltage + 0.0; // Ajusta la f�rmula seg�n tus necesidades
    delay(1000);
    return pHValue;
}

int16_t Sensores::leerLuz(unsigned int canal, float& voltage) {
    int16_t adc = ads1115.readADC_SingleEnded(canal);
    /*Probar si va un asterisco*/ voltage = (adc * 4.096) / 32767;
    // Puedes agregar m�s l�gica aqu� seg�n sea necesario para convertir el valor del ADC a luminosidad
    return adc;
}
