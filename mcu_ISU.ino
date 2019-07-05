#include "DHT.h"
#include <SoftwareSerial.h>
#define DHTPIN 11   
#define DHTTYPE DHT22
#include <Wire.h>
#include <RTClibExtended.h>
#include <LowPower.h>
#define wakePin 2 
#include "HX711.h"

RTC_DS3231 RTC;      //we are using the DS3231 RTC
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial HC12(5, 4); // HC-12 TX Pin, HC-12 RX Pin
HX711 scale1(6, 7);
HX711 scale2(8, 9);
const int npn = 10;
const float calibration_factor1 = 22500;
const float zero_factor1 = -125000;
const float calibration_factor2 = 22500;
const float zero_factor2 = -122000;
float UbatBin; //valeur
float UbatVol; //voltage
float photoResistorBin; //valeur
float photoResistorVol; //voltage
float temperature;
float humidite;
float poids1;
float poids2;
int transmition = 0;
long int sec;
char incomingByte;
int start;

void setup() {
 pinMode(npn, OUTPUT);
 pinMode(wakePin, INPUT);

 Serial.begin(9600);
 Serial.println("initialisation");
 HC12.begin(9600);
 
 dht.begin();
 
 scale1.set_scale(calibration_factor1);
 scale1.set_offset(zero_factor1);
 scale2.set_scale(calibration_factor2);
 scale2.set_offset(zero_factor2);

 Wire.begin();
 RTC.begin(); 
// clear any pending alarms
 RTC.armAlarm(1, false);
 RTC.clearAlarm(1);
 RTC.alarmInterrupt(1, false);
 RTC.armAlarm(2, false);
 RTC.clearAlarm(2);
 RTC.alarmInterrupt(2, false);
 RTC.writeSqwPinMode(DS3231_OFF);
 //Set alarm1 every hours
 RTC.setAlarm(ALM1_MATCH_MINUTES, 0, 0, 0, 0); 
 RTC.alarmInterrupt(1, true);
}

void loop() {
  //preparation a la mise en veille et remise a zero variables
   // Serial.println("extinction des instruments");
  digitalWrite(npn, LOW);
 //   Serial.println("remise a 0 des variables");
 float UbatBin = 0;
 float UbatVol = 0;
 float photoResistorBin = 0;
 float photoResistorVol = 0;
 float temperature = 0;
 float humidite = 0;
 float poids1 = 0;
 float poids2 = 0;
  transmition = 0;
  //veille
   // Serial.println("mise ne veille");
    delay(500);
  attachInterrupt(0, wakeUp, LOW); //debut de la veille
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(0); //fin de la veille
delay(1000);
  RTC.armAlarm(1, false);
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false); //rearmement alarme
 delay(100);
  //  Serial.println("sortie de veille");
  
  //allumage des instruments puis delais attente
  digitalWrite(npn, HIGH);
//    Serial.println("allumage des instruments");
  delay(2000);
 
  
  // recuperation de toutes les donnees des capteurs
//    Serial.println("debut des mesures");
  temperature = dht.readTemperature();
//    Serial.print("temperature ");
  //  Serial.println(temperature);
  humidite = dht.readHumidity();
 //   Serial.print("humidité ");
 //   Serial.println(humidite);
  UbatBin = analogRead(A3);
  //  Serial.print("ubat ");
  //  Serial.println(UbatBin);
  photoResistorBin = analogRead(A2);
   // Serial.print("lum ");
   // Serial.println(photoResistorBin);
  poids1 = scale1.get_units(), 10;
   // Serial.print("poids1 ");
   // Serial.println(poids1);
  poids2 = scale2.get_units(), 10;
   // Serial.print("poids2 ");
   // Serial.println(poids2);

    //Serial.println("fin des mesures");
  //traitement des donnees a traiter
    //Serial.println("debut du traitement");
  UbatVol = voltage(UbatBin);
   // Serial.print("ubatvol ");
   // Serial.println(UbatVol);
  photoResistorVol = voltage(photoResistorBin); 
   // Serial.print("lumvol ");
   // Serial.println(photoResistorVol);

   // Serial.println("fin du traitement");
delay(100);
  //envoie par HC12
   // Serial.println("debut de la transmission");
  while(transmition < 3){
  HC12.print("z");
  delay(10);
  HC12.print("a");
  HC12.print(temperature);
  HC12.print("b");
  HC12.print(humidite);
  HC12.print("c");
  HC12.print(poids1);
  HC12.print("d");
  HC12.print(poids2);
  HC12.print("e");
  HC12.print(UbatVol);
  HC12.print("f");
  HC12.print(photoResistorVol);
  HC12.print("g");
//  Serial.print("z");
  //Serial.print("a");
  //Serial.print(temperature);
  //Serial.print("b");
 // Serial.print(humidite);
 // Serial.print("c");
 // Serial.print(poids1);
 // Serial.print("d");
 // Serial.print(poids2);
 // Serial.print("e");
 // Serial.print(UbatVol);
//  Serial.print("f");
 // Serial.print(photoResistorVol);
 // Serial.print("g   ");
  delay(3000);
  transmition++;
  }
  delay(100);
 // Serial.println("fin de la transmission");
}

float voltage(int valeurBinaire)
{
  return valeurBinaire*0.0048828; 
}

void wakeUp()        // here the interrupt is handled after wakeup
{
}

