#include <Arduino.h>
/*
Código hecho basándome en el tutorial:
https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-servo-motor
Modificado para la tarea particular.

También ocupé un proyecto previo dónde ocupé muchas características similares, para refrescar la memoria
sobre el uso de ciertas funciones y libraries

el repo de ese proyecto es el siguiente: https://github.com/michilakess-eng/smart-bottle-iot

Ocupé Gemini para debugging.
*/

//blynk
#define BLYNK_TEMPLATE_ID "YOUR-TEMPLATE-ID"
#define BLYNK_TEMPLATE_NAME "YOUR-TEMPLATE-NAME"
#define BLYNK_AUTH_TOKEN "YOUR-AUTH-TOKEN"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <time.h>

//wifi
const char ssid[]= "YOUR-WIFI";
const char password[]= "YOUR-WIFI-PASSWORD";

//Hora o NTP
const char ntpServer[]= "pool.ntp.org";
const long gmtOffset_sec= -21600; // timezone CST
const int daylightOffset_sec= 0;

//para después, hardcoded access
String Id_1= " YOUR HEX ID 01";
String Id_2= " YOUR HEX ID 02";
String Id_3= " YOUR HEX ID 03";

#define Servo_PIN D2
#define RST_PIN D3
#define SDA_PIN D8

//millis, timers
unsigned long prevMillis= 0; 
const unsigned long cooldown= 1000;

unsigned long servoH= 0; 
const unsigned long pausaServo= 3000;
bool servoActive= false;

//delete later
int cont= 0;

//libraries 
MFRC522 rfid(SDA_PIN, RST_PIN);
Servo servo;

int angle= 0;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(Servo_PIN);
  servo.write(angle);
  
  Serial.println();
  Serial.print("Conectandose a Blynk y red: ");
  Serial.println(ssid);
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  Serial.println("Blynk y WiFi conectados");
  Blynk.virtualWrite(V2, "Sistema RFID y Blynk listos");
}

void loop() {
  Blynk.run(); 

  unsigned long rnMillis= millis(); //millis del Right Now

  if(rnMillis - prevMillis >= cooldown){
    if (rfid.PICC_IsNewCardPresent()){
      if (rfid.PICC_ReadCardSerial()){
        
        String tagID= "";
        
        //2, ID
        for (int i= 0; i < rfid.uid.size; i++) {
          tagID += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          tagID += String(rfid.uid.uidByte[i], HEX);
        }
        tagID.toUpperCase();

        if(tagID== Id_1 || tagID== Id_2 || tagID== Id_3){
          
          //1, time info
          time_t now = time(nullptr);
          struct tm* timeinfo= localtime(&now);
          char timeStringBuff[50];
          
          if (timeinfo->tm_year > 100){
            strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", timeinfo); //año-mes-día hora:minuto:segundo
          } else {
            strcpy(timeStringBuff, "Sin hora??"); //si falla por alguna razón el ntp
          }

          prevMillis= rnMillis; 
          
          //3, cont
          cont++;

          //string for Blynk. Será enviado de misma o similar manera como JSON al databse.
          String Data= "{\"Fecha y hora\":\"" + String(timeStringBuff) + "\", \"ID\":\"" + tagID + "\", \"Contador\":\"" + String(cont) + "\"}";
          
          //string a Blynk
          Blynk.virtualWrite(V2, Data);

          if (angle== 0){
            angle= 180;
          }
          servo.write(angle);
          servoH= rnMillis;
          servoActive= true;

          rfid.PICC_HaltA(); 
          rfid.PCD_StopCrypto1();
        }
      }
    }
  }
  
  if(servoActive && (rnMillis - servoH >= pausaServo)){
    angle = 0;
    servo.write(angle);
    servoActive = false;
  }
}