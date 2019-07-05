#include "DHT.h"
#include <SoftwareSerial.h>
#define DHTPIN 3   
#define DHTTYPE DHT22
#include <Wire.h>
#include <RTClibExtended.h>
#include <LowPower.h>
#define wakePin 2 
#include "HX711.h"

RTC_DS3231 RTC;      //we are using the DS3231 RTC
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial HC12(4, 5); // HC-12 TX Pin, HC-12 RX Pin
HX711 scale1(6, 7);
HX711 scale2(8, 9);
const int npn = 10;
const float calibration_factor1;
const float zero_factor1;
const float calibration_factor2;
const float zero_factor2;
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
int receive;

void setup() {
 pinMode(npn, OUTPUT);
 pinMode(wakePin, INPUT);
 
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
  digitalWrite(npn, LOW);
  UbatBin = 0;
  UbatVol = 3.75;
  photoResistorBin = 0;
  photoResistorVol = 4.25;
  temperature = 20.24;
  humidite = 19.56;
  poids1 = 204.20;
  poids2 = 109.50;
  transmition = 0;
  //veille
  attachInterrupt(0, wakeUp, LOW); //debut de la veille
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(0); //fin de la veille
  RTC.armAlarm(1, true);
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, true); //rearmement alarme
  delay(1);
  
  //allumage des instruments puis delai attente
  digitalWrite(npn, HIGH);
  delay(2000);
  
  // recuperation de toutes les donnees des capteurs
  temperature = dht.readTemperature();
  humidite = dht.readHumidity();
  UbatBin = analogRead(A3);
  photoResistorBin = analogRead(A2);
  poids1 = scale1.get_units(), 5;
  poids2 = scale2.get_units(), 5;
  
  //traitement des donnees a traiter
  UbatVol = voltage(UbatBin);
  photoResistorVol = voltage(photoResistorBin); 

  //envoie par HC12
  while(transmition < 2){
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
  delay(10);
  sec = millis();
  transmition++;
  }
}

float voltage(int valeurBinaire)
{
  return valeurBinaire*0.0048828; 
}

void wakeUp()        // here the interrupt is handled after wakeup
{
}

