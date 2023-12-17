#define power_pin 5

#include <Adafruit_ADS1X15.h>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Definir para conectar a Internet:
#include <ESP8266WiFi.h>

// Comentar/Descomentar para ver mensajes de depuracion en monitor serie y/o respuesta del HTTP server
#define PRINT_DEBUG_MESSAGES
#define PRINT_HTTP_RESPONSE

// Comentar/Descomentar para conexion Fuera/Dentro de UPV
//#define WiFi_CONNECTION_UPV

// Selecciona que servidor REST quieres utilizar entre ThingSpeak y Dweet
#define REST_SERVER_THINGSPEAK //Selecciona tu canal para ver los datos en la web (https://thingspeak.com/channels/360979)
//#define REST_SERVER_DWEET //Selecciona tu canal para ver los datos en la web (http://dweet.io/follow/PruebaGTI)

//////////////////////////////////////////////////////
/////////////// WiFi Definitions /////////////////////
//////////////////////////////////////////////////////

#ifdef WiFi_CONNECTION_UPV //Conexion UPV
  const char WiFiSSID[] = "GTI1";
  const char WiFiPSK[] = "1PV.arduino.Toledo";
#else //Conexion fuera de la UPV
  const char WiFiSSID[] = "Redmi";
  const char WiFiPSK[] = "45678901";
#endif



///////////////////////////////////////////////////////
/////////////// SERVER Definitions ///////////////////
//////////////////////////////////////////////////////

#if defined(WiFi_CONNECTION_UPV) //Conexion UPV
  const char Server_Host[] = "proxy.upv.es";
  const int Server_HttpPort = 8080;
#elif defined(REST_SERVER_THINGSPEAK) //Conexion fuera de la UPV
  const char Server_Host[] = "api.thingspeak.com";
  const int Server_HttpPort = 80;
#else
  const char Server_Host[] = "dweet.io";
  const int Server_HttpPort = 80;
#endif

WiFiClient client;

/////////////////////////////////////////////////////
/////////////// HTTP REST Connection ////////////////
/////////////////////////////////////////////////////

#ifdef REST_SERVER_THINGSPEAK 
  const char Rest_Host[] = "api.thingspeak.com";
  String MyWriteAPIKey="A307YHHDJTMOH6DG"; // Escribe la clave de tu canal ThingSpeak
#else 
  const char Rest_Host[] = "dweet.io";
  String MyWriteAPIKey="cdiocurso2023g08"; // Escribe la clave de tu canal Dweet
#endif

#define NUM_FIELDS_TO_SEND 2 //Numero de medidas a enviar al servidor REST (Entre 1 y 8)

////////////////////////////////////////////////
/////////////// Pin Definitions ////////////////
////////////////////////////////////////////////

const int LED_PIN = 5; // Thing's onboard, green LED

////////////////////////////////////////////////
/////////////// WiFi Connection ////////////////
////////////////////////////////////////////////

void connectWiFi()
{
  byte ledStatus = LOW;

  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
  #endif
  
  WiFi.begin(WiFiSSID, WiFiPSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    #ifdef PRINT_DEBUG_MESSAGES
       Serial.println(".");
    #endif
    delay(500);
  }
  #ifdef PRINT_DEBUG_MESSAGES
     Serial.println( "WiFi Connected" );
     Serial.println(WiFi.localIP()); // Print the IP address
  #endif
}

/////////////////////////////////////////////////////
/////////////// HTTP POST  ThingSpeak////////////////
/////////////////////////////////////////////////////

void HTTPPost(String fieldData[], int numFields){

// Esta funcion construye el string de datos a enviar a ThingSpeak mediante el metodo HTTP POST
// La funcion envia "numFields" datos, del array fieldData.
// Asegurate de ajustar numFields al número adecuado de datos que necesitas enviar y activa los campos en tu canal web
  
    if (client.connect( Server_Host , Server_HttpPort )){
       
        // Construimos el string de datos. Si tienes multiples campos asegurate de no pasarte de 1440 caracteres
        String PostData= "api_key=" + MyWriteAPIKey ;
        for ( int field = 1; field < (numFields + 1); field++ ){
            PostData += "&field" + String( field ) + "=" + fieldData[ field ];
        }     
        
        // POST data via HTTP
        #ifdef PRINT_DEBUG_MESSAGES
            Serial.println( "Connecting to ThingSpeak for update..." );
        #endif
        client.println( "POST http://" + String(Rest_Host) + "/update HTTP/1.1" );
        client.println( "Host: " + String(Rest_Host) );
        client.println( "Connection: close" );
        client.println( "Content-Type: application/x-www-form-urlencoded" );
        client.println( "Content-Length: " + String( PostData.length() ) );
        client.println();
        client.println( PostData );
        #ifdef PRINT_DEBUG_MESSAGES
            Serial.println( PostData );
            Serial.println();
            //Para ver la respuesta del servidor
            #ifdef PRINT_HTTP_RESPONSE
              delay(500);
              Serial.println();
              while(client.available()){String line = client.readStringUntil('\r');Serial.print(line); }
              Serial.println();
              Serial.println();
            #endif
        #endif
    }
}

////////////////////////////////////////////////////
/////////////// HTTP GET  //////////////////////////
////////////////////////////////////////////////////

void HTTPGet(String fieldData[], int numFields){
  
// Esta funcion construye el string de datos a enviar a ThingSpeak o Dweet mediante el metodo HTTP GET
// La funcion envia "numFields" datos, del array fieldData.
// Asegurate de ajustar "numFields" al número adecuado de datos que necesitas enviar y activa los campos en tu canal web
  
    if (client.connect( Server_Host , Server_HttpPort )){
           #ifdef REST_SERVER_THINGSPEAK 
              String PostData= "GET https://api.thingspeak.com/update?api_key=";
              PostData= PostData + MyWriteAPIKey ;
           #else 
              String PostData= "GET http://dweet.io/dweet/for/";
              PostData= PostData + MyWriteAPIKey +"?" ;
           #endif
           
           for ( int field = 1; field < (numFields + 1); field++ ){
              PostData += "&field" + String( field ) + "=" + fieldData[ field ];
           }
          
           
           #ifdef PRINT_DEBUG_MESSAGES
              Serial.println( "Connecting to Server for update..." );
           #endif
           client.print(PostData);         
           client.println(" HTTP/1.1");
           client.println("Host: " + String(Rest_Host)); 
           client.println("Connection: close");
           client.println();
           #ifdef PRINT_DEBUG_MESSAGES
              Serial.println( PostData );
              Serial.println();
              //Para ver la respuesta del servidor
              #ifdef PRINT_HTTP_RESPONSE
                delay(500);
                Serial.println();
                while(client.available()){String line = client.readStringUntil('\r');Serial.print(line); }
                Serial.println();
                Serial.println();
              #endif
           #endif  
    }
}

Adafruit_ADS1115 ads1115;


void setup() {
 
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.begin(9600);
  #endif
  
  connectWiFi();
  digitalWrite(LED_PIN, HIGH);

  #ifdef PRINT_DEBUG_MESSAGES
      Serial.print("Server_Host: ");
      Serial.println(Server_Host);
      Serial.print("Port: ");
      Serial.println(String( Server_HttpPort ));
      Serial.print("Server_Rest: ");
      Serial.println(Rest_Host);
  #endif
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


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Definir los canales de los sensores
#define canalluz 3

#define canalPH 2

#define canalTemperatura 1//Añadir canal aquí

#define canalHumedad 0// Añadir canal aquí

#define offset 0.00

#define samplingInterval 20

#define printInterval 800

#define ArrayLength 40

int pHArray[ArrayLength];

int pHArrayIndex = 0;

// Inicializar el objeto Adafruit_ADS1115



// Valores de humedad

const int valorEnSeco = 30500;

const int valorEnAgua = 17000;


float funcionHumedad(unsigned int canalAdc) {

 int16_t adc0;

 int humedad;
 
 adc0 = ads1115.readADC_SingleEnded(canalAdc);
 
 humedad = map(adc0, valorEnSeco, valorEnAgua, 0, 100);
 
 if (humedad > 100) {

 humedad = 100;

 }
 
 Serial.println(adc0);
 
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
int funcionSalinidad() {

 int16_t adc5;
 
 digitalWrite(power_pin, HIGH);

 delay(100);

 adc5 = analogRead(A0); // Cambiado para usar el ADS1115

 digitalWrite(power_pin, LOW);

 delay(100);
 
 int salinidad = map (adc5, 430, 900, 0, 100);
 
 if(salinidad > 100){
  
  salinidad=100;
  
 }
 
 if(salinidad<0){
  
 salinidad=0;
 
 }
 
 return salinidad;

}

int funcionPH (unsigned int canalAdc) {
  
 static float pHValue, voltage;
 
 voltage = (ads1115.readADC_SingleEnded(canalAdc) * 4.096) / (pow(2 , 15) - 1);

 pHValue = 3.5 * voltage + 0;
 
 Serial.print (" pH Value: ");
 
 Serial.println (pHValue, 2);
 
 delay(1000);
 
 return pHValue;
 
}

int16_t funcLuz(unsigned int canalAdc, float  V ){
  int16_t adc;
  adc = ads1115.readADC_SingleEnded(canalAdc);
   static float Vout;
  Vout = ((4.096) * adc) / (32767); //formula Luminositat
  V = Vout;
  return adc;
}

void loop() {
 float Temperatura, Humedad, Salinidad, PH, Luz, V;

 Salinidad = funcionSalinidad();

 Temperatura = funcionTemperatura(canalTemperatura);

 Humedad = funcionHumedad(canalHumedad);
 
 PH = funcionPH(canalPH);
 
 Luz = funcLuz(canalluz, V);


    String data[ NUM_FIELDS_TO_SEND + 1];  // Podemos enviar hasta 8 datos
    data[ 1 ] = Temperatura  ); //Escribimos el dato 1. Recuerda actualizar numFields
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Temperatura: " );
        Serial.println( data[ 1 ] );
    #endif

    data[ 2 ] = Humedad ); //Escribimos el dato 2. Recuerda actualizar numFields
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Humedad:" );
        Serial.println( data[ 2 ] );
    #endif

    data[ 3 ] = Salinidad ); //Escribimos el dato 2. Recuerda actualizar numFields
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Salinidad:" );
        Serial.println( data[ 2 ] );
    #endif

    data[ 4 ] = PH ); //Escribimos el dato 2. Recuerda actualizar numFields
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "PH:" );
        Serial.println( data[ 4 ] );
    #endif

    data[ 5 ] = V ); //Escribimos el dato 2. Recuerda actualizar numFields
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Luminosidad:" );
        Serial.println( data[ 5 ] );
    #endif
    //Selecciona si quieres enviar con GET(ThingSpeak o Dweet) o con POST(ThingSpeak)
    //HTTPPost( data, NUM_FIELDS_TO_SEND );
    HTTPGet( data, NUM_FIELDS_TO_SEND );

    //Selecciona si quieres un retardo de 15seg para hacer pruebas o dormir el SparkFun
    delay( 1000 );   
    //Serial.print( "Goodnight" );
    //ESP.deepSleep( sleepTimeSeconds * 1000000 );

 
 
 // Imprimir los valores
 
 //Humedad:
 Serial.print("La humedad es: ");
 
 Serial.print(Humedad);
 
 Serial.println("%");
 
 Serial.print("Canal: 0");
 
 Serial.println(canalHumedad);
 
 delay(1000);
 
 //Temperatura:
 
 Serial.print("La temperatura es de: ");
 
 Serial.print(Temperatura);
 
 Serial.println("ºC");
 
 Serial.print("Canal: 1");
 
 Serial.println(canalTemperatura);
 
 delay(1000);
 
 //Salinidad:
 
 Serial.print("Porcentaje de salinidad: ");
 
 Serial.print(Salinidad);
 
 Serial.println("%");
 
 Serial.println("Canal: ADS");
 
 delay(1000);
 
 //PH
 
 Serial.print("PH: ");
 
 Serial.print(PH);
 
 Serial.println("4 y 7");
 
 Serial.println("Canal: 2");
 
 delay(1000);

  //Luz
  Serial.print("Luminosidad: ");
 
 Serial.print(Luz);
 
 Serial.println("");
 
 Serial.println("Canal: 3");
 
 delay(1000);
 
}
