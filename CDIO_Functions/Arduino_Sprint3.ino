#include "libreria.h"
const int power_pin = 5; // Replace with the actual pin number
Sensores sensores(power_pin);
#include <ESP8266WiFi.h>
#include <Adafruit_ADS1X15.h>
#define PRINT_DEBUG_MESSAGES
#define PRINT_HTTP_RESPONSE

// Comentar/Descomentar para conexion Fuera/Dentro de UPV
//#define WiFi_CONNECTION_UPV

// Selecciona que servidor REST quieres utilizar entre ThingSpeak y Dweet
#define REST_SERVER_THINGSPEAK //Selecciona tu canal para ver los datos en la web (https://thingspeak.com/channels/360979)
//#define REST_SERVER_DWEET //Selecciona tu canal para ver los datos en la web (http://dweet.io/follow/PruebaGTI)

//WiFi Definitions
#ifdef WiFi_CONNECTION_UPV //Conexion UPV
  const char WiFiSSID[] = "GTI1";
  const char WiFiPSK[] = "1PV.arduino.Toledo";
#else //Conexion fuera de la UPV
  const char WiFiSSID[] = "Redmi";
  const char WiFiPSK[] = "45678901";
#endif

//SERVER Definitions
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


//REST
#ifdef REST_SERVER_THINGSPEAK 
  const char Rest_Host[] = "api.thingspeak.com";
  String MyWriteAPIKey="A307YHHDJTMOH6DG"; // Escribe la clave de tu canal ThingSpeak
#else 
  const char Rest_Host[] = "dweet.io";
  String MyWriteAPIKey="cdiocurso2023g08"; // Escribe la clave de tu canal Dweet
#endif
#define NUM_FIELDS_TO_SEND 5 //Numero de medidas a enviar al servidor REST (Entre 1 y 8)

//Pin Definitions
const int LED_PIN = 5; // Thing's onboard, green LED

//WiFi Connection
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

//POST
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

//GET
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
  pinMode(power_pin, OUTPUT);
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print("Server_Host: ");
    Serial.println(Server_Host);
    Serial.print("Port: ");
    Serial.println(String( Server_HttpPort ));
    Serial.print("Server_Rest: ");
    Serial.println(Rest_Host);
  #endif

  // Inicializar el puerto serie para la depuración
  Serial.println("Inicializando...");

  // Iniciar el ADS1115
  ads1115.begin(0x48);

  // Configurar la ganancia del ADS1115
  ads1115.setGain(GAIN_ONE);
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

    Serial.println("Valores que se van a enviar a la nube: ");
  Serial.println("---------------------------------------------");
  String data[ NUM_FIELDS_TO_SEND + 1];  // Podemos enviar hasta 8 datos

  //Envio de datos de Temperatura
  data[ 1 ] = ( temperatura  );
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print( "Temperatura: " );
    Serial.print( data[ 1 ] );
    Serial.println("ºC");
  #endif

  //Envio de datos de Humedad
  data[ 2 ] = ( humedad ); 
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print( "Humedad: " );
    Serial.print( data[ 2 ] );
    Serial.println("%");
  #endif

  //Envio de datos de Salinidad
  data[ 3 ] = ( salinidad );
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print( "Salinidad: " );
    Serial.print( data[ 3 ] );
    Serial.println("%");
  #endif

  //Envio de datos de PH
  data[ 4 ] = ( pH );
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print( "PH: " );
    Serial.println( data[ 4 ] );
  #endif

  //Envio de datos de Luz
  data[ 5 ] = ( luz );
  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print( "Luminosidad:" );
    Serial.println( data[ 5 ] );
  #endif
  Serial.println("---------------------------------------------");
  Serial.println(" ");
  //Selecciona si quieres enviar con GET(ThingSpeak o Dweet) o con POST(ThingSpeak)
  //HTTPPost( data, NUM_FIELDS_TO_SEND );
  HTTPGet( data, NUM_FIELDS_TO_SEND );

  //Selecciona si quieres un retardo de 15seg para hacer pruebas o dormir el SparkFun
  delay( 1000 );   
  //Serial.print( "Goodnight" );
  //ESP.deepSleep( sleepTimeSeconds * 1000000 );
    
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
