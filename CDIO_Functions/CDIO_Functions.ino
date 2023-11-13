#define power_pin 5
#include <Adafruit_ADS1X15.h>

// Definir los canales de los sensores

#define canalTemperatura 1//Añadir canal aquí

#define canalHumedad 0// Añadir canal aquí

// Inicializar el objeto Adafruit_ADS1115

Adafruit_ADS1115 ads1115;

// Valores de humedad

const int valorEnSeco = 30500;

const int valorEnAgua = 17000;

void setup() {
  
  // Inicializar el puerto serie para la depuración
  
  Serial.begin(9600);
  
  Serial.println("Inicializando...");
  
  // Iniciar el ADS1115
  
  ads1115.begin();
  
  // Configurar la ganancia del ADS1115
  
  ads1115.setGain(GAIN_ONE);
  
  Serial.println("Ajustando la ganancia...");
  
  Serial.println("Tomando medidas del canal AIN0");
  
  Serial.println("Rango del ADC: +/- 4.096V (1 bit = 2mV)");
  
}

float funcionHumedad(unsigned int canalAdc) {
  
  int16_t adc0;
  
  int humedad;

  adc0 = ads1115.readADC_SingleEnded(canalAdc);

  humedad = map(adc0, valorEnSeco, valorEnAgua, 0, 100);

  if (humedad > 100) {
    
    humedad = 100;
    
  }

  return humedad;
  
}

float funcionTemperatura(unsigned int canalAdc) {
  
  int16_t adc1 = ads1115.readADC_SingleEnded(canalAdc);
  
  float b = 0.79;
  
  float m = 0.033;
  
  float vo = (adc1 * 0.000125); // 4.096V / 32767 = 0.000125
  
  float valorTemperatura = ((vo - b) / m) - 2;

  return valorTemperatura;
  
}

float funcionSalinidad() {
  
  int16_t adc5;

  digitalWrite(power_pin, HIGH);
  
  delay(100);
  
  adc5 = ads1115.readADC_SingleEnded(5); // Cambiado para usar el ADS1115
  
  digitalWrite(power_pin, LOW);
  
  delay(100);

  float valorSalinidad = 0.0000007 * pow(adc5, 3) - 0.0006 * pow(adc5, 2) + 0.114 * adc5;

  if (adc5 == 573) {
    
    valorSalinidad = 0;
    
  }

  return valorSalinidad;
  
}

void loop() {
  
  float Temperatura, Humedad, Salinidad;
  
  Salinidad = funcionSalinidad();
  
  Temperatura = funcionTemperatura(canalTemperatura);
  
  Humedad = funcionHumedad(canalHumedad);

  // Imprimir los valores
  //Humedad:
  Serial.print("La humedad es: ");
  Serial.print(Humedad);
  Serial.println("%");
  Serial.print("Canal: ");
  Serial.println(canalHumedad);
  delay(1000);

  //Temperatura:
  Serial.print("La temperatura es: ");
  Serial.print(Temperatura);
  Serial.println("ºC");
  Serial.print("Canal: ");
  Serial.println(canalTemperatura);
  delay(1000);

  //Salinidad:
  Serial.print("Porcentaje de salinidad: ");
  Serial.print(Salinidad);
  Serial.println("%");
  Serial.println("Canal: 5");
  delay(3000);
}