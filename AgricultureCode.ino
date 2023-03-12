#include <DHT.h>  // Including library for dht

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

String apiKey = "DUTZS1W7JTEZ0LZV";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "Hruday";     // replace with your wifi ssid and wpa2 key
const char *pass =  "hruday@23";
const char* server = "api.thingspeak.com";

#define DHTPIN 0          //pin where the dht11 is connected
#define SENSOR  2

DHT dht(DHTPIN, DHT11);
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;
int moisture_percentage;
int sensor_pin=A0;
int pir_sensor=4;







 
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
WiFiClient client;
 
void setup() 
{
       Serial.begin(9600);
       delay(10);
       dht.begin();
      
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      pinMode(SENSOR, INPUT_PULLUP);
      pinMode(pir_sensor,INPUT); 
         
 
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  moisture_percentage=0;
 
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
 
}
 
void loop() 
{
 
      currentMillis = millis();
      int pir=digitalRead(pir_sensor);
      
        if (currentMillis - previousMillis > interval) 
          {
            pulse1Sec = pulseCount;
            pulseCount = 0;
            flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
            previousMillis = millis();
            flowMilliLitres = (flowRate / 60) * 1000;
            flowLitres = (flowRate / 60);
            totalMilliLitres += flowMilliLitres;
            totalLitres += flowLitres;
            moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
            


    
            float h = dht.readHumidity();
            float t = dht.readTemperature();
      
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }


                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "&field3=";
                             postStr += String(float(flowRate));
                             postStr += "&field3=";
                             postStr += String(totalLitres);
                             postStr += "&field4=";
                             postStr += String(moisture_percentage);
                             postStr += "&field5=";
                             postStr += String(pir);
                             postStr += "r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.println(h);



                             
                             Serial.print("Flow rate: ");
                             Serial.print(float(flowRate));  // Print the integer part of the variable
                             Serial.print("L/min");
                             Serial.print("\t"); 
                          
 
    // Print the cumulative total of litres flowed since starting
                             Serial.print("Output Liquid Quantity: ");
                             Serial.print(totalMilliLitres);
                             Serial.print("mL / ");
                             Serial.print(totalLitres);
                             Serial.println("L");
                             Serial.print("Outpur total mositure ");
                             Serial.print(moisture_percentage);
                             Serial.print("%");
                             Serial.print("motion :");
                             Serial.print(pir);
                             Serial.print("\n");      // Print tab space

                             Serial.println("%. Send to Thingspeak.");
                             
                             
                        }
          client.stop();
 
          Serial.println("Waiting...");
          }
          }
