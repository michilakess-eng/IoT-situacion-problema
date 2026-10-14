/*
Código hecho basándome en el tutorial:
https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-servo-motor
Modificado para la tarea particular.

Aún falta agregar un counter que pueda determinar cuántas veces se ha ingresado.
*/
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define Servo_PIN 2
#define RST_PIN 4
#define MISO_PIN 21
#define MOSI_PIN 19
#define SCK_PIN 23
#define SDA_PIN 18

//cooldown, pausa
unsigned long prevMillis = 0; 
const unsigned long cooldown = 1000;

unsigned long servoH= 0; //srvo on millis
const unsigned long pausaServo = 3000;
bool servoActive= false;

//cont
int cont= 0;

//declare
MFRC522 rfid(SDA_PIN, RST_PIN);
Servo servo;

int angle = 0;

void setup() {
  Serial.begin(115200);//sale en 9600 en el otro documento, checar bien qué numero corresponde
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(Servo_PIN);
  servo.write(angle);
  
  Serial.println("RFID listo");
}

// put your main code here, to run repeatedly:
void loop() {
  unsigned long rnMillis = millis(); //millis del Right Now

  if(rnMillis - prevMillis >= cooldown){
    if (rfid.PICC_IsNewCardPresent()){
      if (rfid.PICC_ReadCardSerial()){
        prevMillis = rnMillis; 

        // print NUID in Serial Monitor in the hex format
        Serial.print("ID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        cont++;
        Serial.println();
        Serial.print("Accesos: ");
        Serial.println(cont);

        //srvo
         if (angle == 0){
          angle = 180;
        }
        servo.write(angle);
        servoH= rnMillis;
        servoActive= true;

        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
      }
    }
  }
  if(servoActive && (rnMillis - servoH >= pausaServo)){
    angle=0;
    servo.write(angle);
    servoActive= false;
  }

}