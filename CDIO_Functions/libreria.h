// sensores.h

#ifndef SENSORES_H

#define SENSORES_H

#include <Adafruit_ADS1X15.h>

class Sensores {

public:

    Sensores(int powerPin);

    float leerTemperatura(unsigned int canal);

    float leerHumedad(unsigned int canal);

    int leerSalinidad();

    int leerPH(unsigned int canal);

    int16_t leerLuz(unsigned int canal, float& voltage);

private:

    int powerPin;

    Adafruit_ADS1115 ads1115;

};

#endif  // SENSORES_H
