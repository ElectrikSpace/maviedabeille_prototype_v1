    #include <SoftwareSerial.h>
    #include "LowPower.h"

    SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
    SoftwareSerial ESP8266(5, 6);

    String NomduReseauWifi = "Entrez le nom de votre Box ou point d'accès Wifi"; // Garder les guillements
    String MotDePasse      = "Entrez le nom du mot de passe de votre Box ou point d'accès Wifi"; // Garder les guillements
    String httpGet = "";
    int taille;
    int hcend = 0;
    int variableWrite = 0;
    long int temps;
    char incomingByte;
    String temperature;
    String humidite;
    String luminosite;
    String voltage;
    String poids1;
    String poids2;
    int i;
    int espPin = 6;

    void setup()
    {
      pinMode(espPin, OUTPUT);
      ESP8266.begin(9600);  
      HC12.begin(9600);
      Serial.begin(9600);
      envoieAuESP8266("AT+RST");
      recoitDuESP8266(2000);
    }
   
    void loop()
    {
     digitalWrite(espPin, HIGH); 
      //reception donnees de HC12
      while( (temps+600000) > millis() && hcend == 0){
        while (HC12.available()) {
          incomingByte = HC12.read();
         // Serial.print(incomingByte);
           if (isAlpha(incomingByte)) {
            switch (incomingByte) {
              case 'g':
                hcend = 1;
                variableWrite = 0;
              break;
              case 'z':
                variableWrite = 0;
              break;
              case 'a':
                variableWrite = 1;
              break;
              case 'b':
                variableWrite = 2;
              break;
              case 'c':
                variableWrite = 3;
              break;
              case 'd':
                variableWrite = 4;
              break;
              case 'e':
                variableWrite = 5;
              break;
              case 'f':
                variableWrite = 6;
              break;
              default:
              break;
            }
           }
         else {
            switch (variableWrite) {
             case 1:
                temperature += incomingByte;
              break; 
             case 2:
                humidite += incomingByte;
              break; 
             case 3:
                poids1 += incomingByte;
              break; 
             case 4:
                poids2 += incomingByte;
              break; 
             case 5:
                voltage += incomingByte;
              break; 
             case 6:
                luminosite += incomingByte;
              break; 
             default:  
              break;   
            }
         }
           }
   }
   if (hcend == 1){
    HC12.print("y");
   }
   else if (hcend == 0){
    HC12.print("n");
   }
      //Serial.print("temperature");
     // Serial.print(temperature);
     // Serial.print("humidite");
     // Serial.print(humidite);
     // Serial.print("poids1");
     // Serial.print(poids1);
     // Serial.print("poids2");
     // Serial.print(poids2);
      //Serial.print("voltage");
     // Serial.print(voltage);
     // Serial.print("luminosite");
     // Serial.print(luminosite);
     
      //envoi par wifi
      httpGet = String("GET /emoncms/input/post?node=emontx&fulljson={\"poidsruche1\":" + poids1 + ",\"poidsruche2\":" + poids2 + ",\"temperature\":" + temperature + ",\"humidite\":" + humidite + ",\"luminosite\":" + luminosite + ",\"voltagebatterie\":" + voltage + "}&apikey=827120f283dcac11ff6094a86daa0995&HTTP/1.1");
      taille = httpGet.length() + 4;
      Serial.println(httpGet);
      initESP8266();
      envoieAuESP8266("AT+CWMODE=1");
      recoitDuESP8266(5000);
      envoieAuESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\"");
      recoitDuESP8266(15000);
      envoieAuESP8266("AT+CIPSTART=\"TCP\",\"78.200.224.32\",80");
      recoitDuESP8266(3000);
      envoieAuESP8266("AT+CIPSEND=" + taille);   
      recoitDuESP8266(1000);
      envoieAuESP8266(httpGet);
      recoitDuESP8266(1000);
      envoieAuESP8266("AT+CIPCLOSE");
      recoitDuESP8266(2000);
      envoieAuESP8266("AT+CWQAP");
      recoitDuESP8266(1000); 
      delay(100);
      digitalWrite(espPin, LOW);

    //remise à zero
    temperature = "";
    humidite = "";
    luminosite = "";
    voltage = "";
    poids1   = "";
    poids2 = "";

    //mise en veille
    for (i=0; i <= 412; i++){
      delay(1);
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
   }
   delay(100); 
    }
   
    void initESP8266()
    {  
      envoieAuESP8266("AT+RST");
      recoitDuESP8266(2000);
    }

    void envoieAuESP8266(String commande)
    {  
      ESP8266.println(commande);
    }
 
    void recoitDuESP8266(const int timeout)
    {
      String reponse = "";
      long int time = millis();
      while( (time+timeout) > millis())
      {
        while(ESP8266.available())
        {
          char c = ESP8266.read();
          reponse+=c;
        }
      }
      Serial.print(reponse);   
    }

