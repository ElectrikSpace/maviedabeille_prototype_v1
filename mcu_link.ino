    #include <SoftwareSerial.h>
    #include "LowPower.h"

    SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
    SoftwareSerial ESP8266(5, 6);

    String NomduReseauWifi = "Freebox-5F5237"; // Garder les guillements
    String MotDePasse = "exectus-fictionem.2-feris2-leunculo@"; // Garder les guillements
    String taille;
    String taille2;
    String taille3;
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
    int mosfet = 10;

    void setup()
    {
      pinMode(mosfet, OUTPUT);
      digitalWrite(mosfet, HIGH);
      Serial.begin(9600);
      ESP8266.begin(115200); 
      initESP8266();  
      HC12.begin(9600);
    }
   
    void loop()
    {
      //reception donnees de HC12
 //     Serial.println("reception des donnees");
      HC12.listen();
      int hcend = 0;
      long int temps = millis(); 
      while( (temps+3900000) > millis() && hcend < 2){
        while (HC12.available()) {
          incomingByte = HC12.read();
         // Serial.print(incomingByte);
           if (isAlpha(incomingByte)) {
            switch (incomingByte) {
              case 'g':
                hcend = 3;
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
  
      Serial.print("temperature");
      Serial.println(temperature);
      Serial.print("humidite");
      Serial.println(humidite);
      Serial.print("poids1");
      Serial.println(poids1);
      Serial.print("poids2");
      Serial.println(poids2);
      Serial.print("voltage");
      Serial.println(voltage);
      Serial.print("luminosite");
      Serial.println(luminosite);
     
      
      ESP8266.listen();
      Serial.println("**********************************************************");
      envoieAuESP8266("AT+CWMODE=1");
      recoitDuESP8266(5000);
      Serial.println("**********************************************************");
      envoieAuESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\"");
      recoitDuESP8266(10000);
      Serial.println("**********************************************************");
      Serial.println("**********************************************************");
      Serial.println("***************** INITIALISATION TERMINEE ****************");
      Serial.println("**********************************************************");
     
  //    String test = "GET /emoncms/input/post?node=emontx&fulljson={\"poidsruche1\":" + poids1 + ",\"poidsruche2\":" + poids2 + ",\"temperature\":" + temperature + ",\"humidite\":" + humidite + ",\"luminosite\":" + luminosite + ",\"voltagebatterie\":" + voltage + "}&apikey=827120f283dcac11ff6094a86daa0995&HTTP/1.1";
      String httpget = "GET /emoncms/input/post?node=emontx&fulljson={\"poidsruche1\":" + poids1 + ",\"poidsruche2\":" + poids2 + ",\"voltagebatterie\":" + voltage +  ",\"luminosite\":" + luminosite + "}&apikey=827120f283dcac11ff6094a86daa0995&HTTP/1.1";
     // String httpget3 = "GET /emoncms/input/post?node=emontx&fulljson={\"humidite\":" + humidite + ",\"temperature\":" + temperature + ",\"luminosite\":" + luminosite + "}&apikey=827120f283dcac11ff6094a86daa0995&HTTP/1.1";
      delay(100);
      taille = httpget.length() + 4;
     
     
    //  Serial.println(httpget);
     // Serial.println(httpget2);
     // Serial.println(httpget3);
      delay(10);
      envoieAuESP8266("AT+CIPSTART=\"TCP\",\"78.200.224.32\",80");
      recoitDuESP8266(3000);
      envoieAuESP8266("AT+CIPSEND=" + taille);   
      recoitDuESP8266(1000);
      delay(100);
      envoieAuESP8266(httpget);
      recoitDuESP8266(1000);
      envoieAuESP8266("AT+CIPCLOSE");
      recoitDuESP8266(2000);
      String httpget2 = "GET /emoncms/input/post?node=emontx&fulljson={\"temperature\":" + temperature + ",\"humidite\":" + humidite + "}&apikey=827120f283dcac11ff6094a86daa0995&HTTP/1.1";
      taille2 = httpget2.length() + 4;
      envoieAuESP8266("AT+CIPSTART=\"TCP\",\"78.200.224.32\",80");
      recoitDuESP8266(3000);
      envoieAuESP8266("AT+CIPSEND=" + taille2);   
      recoitDuESP8266(1000);
     // delay(100);
      envoieAuESP8266(httpget2);
      recoitDuESP8266(1000);
      envoieAuESP8266("AT+CIPCLOSE");
      recoitDuESP8266(2000); 
    /*  httpget = "";
      httpget2 = "";
      delay(100);
      String httpget3 = "GET /emoncms/input/post?node=emontx&fulljson={\"voltagebatterie\":" + voltage + "}&apikey=827120f283dcac11ff6094a86daa0995&HTTP/1.1";
       delay(100);
      taille3 = httpget3.length() + 4;
      envoieAuESP8266("AT+CIPSTART=\"TCP\",\"78.200.224.32\",80");
      recoitDuESP8266(3000);
      envoieAuESP8266("AT+CIPSEND=" + taille3);   
      recoitDuESP8266(1000);
      delay(100);
      envoieAuESP8266(httpget3);
      recoitDuESP8266(1000);
      envoieAuESP8266("AT+CIPCLOSE");
      recoitDuESP8266(2000);
       httpget3 = ""; */
      envoieAuESP8266("AT+CWQAP");
      recoitDuESP8266(2000);
      Serial.println("fin de l envoi");
      delay(100);
      Serial.println("extinction des instruments");
       delay(100);
     // digitalWrite(mosfet, LOW);

    //remise à zero
      Serial.println("remise a zero des variables");
    httpget = "";
    httpget2 = "";
//    httpget3 = "";
    temperature = "";
    humidite = "";
    luminosite = "";
    voltage = "";
    poids1 = "";
    poids2 = "";
    hcend = 0;
       delay(100);
   
    } 
   
    void initESP8266()
    {  
      Serial.println("**********************************************************");  
      Serial.println("**************** DEBUT DE L'INITIALISATION ***************");
      Serial.println("**********************************************************");  
      envoieAuESP8266("AT+RST");
      recoitDuESP8266(2000);
      Serial.println("**********************************************************");
      Serial.println("**********************************************************");
      Serial.println("***************** INITIALISATION TERMINEE ****************");
      Serial.println("**********************************************************");
      Serial.println(""); 
    }

   void envoieAuESP8266(String commande)
    {  
      ESP8266.println(commande);
    }
    /****************************************************************/
    /*Fonction qui lit et affiche les messages envoyés par l'ESP8266*/
    /****************************************************************/
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
