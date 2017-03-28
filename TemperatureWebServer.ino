#include <ESP8266WebServer.h>
#include <MQ2.h>
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>    //DHT11 DHT22 
//#define DHTTYPE DHT11   // DHT 11
const char* ssid = "ok";
const char* password = "okokokok";

ESP8266WebServer server(80);
//const int DHTPin = D4;  //DHT11 Data Pin
// Initialize DHT sensor.
DHT dht(D4, DHT11);   // intialize our DHT11
int pin = D2;
int pin2 = D0;
MQ2 mq2(pin);
char temperatureString[6];
char temperatureString2[6];
static char celsiusTemp[7];
//const int buzzer = D0;

void setup(void){
  Serial.begin(115200);   //Serial Communication Begin
  delay(10);
  dht.begin();   // dht monitoring
  mq2.begin();
  WiFi.begin(ssid, password); //Conecting it to WiFi
  Serial.println(""); // Message that device is Connected or Not

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  server.begin();      //ES8266 Webserver
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", []() {
    float temperature = dht.readTemperature();
    dtostrf(temperature, 2, 2, temperatureString);
    float temperature2 = dht.readHumidity();
    dtostrf(temperature2, 2, 2, temperatureString2);
  
    String title = "Temperature";
    String title2 = "Humidity";
    String title3 = "Heat Index";
    String title4 = "LPG";
    String title5 = "CO";
    String title6 = "smoke";
    String cssClass = "mediumhot";
    if (temperature < 15)
      cssClass = "cold";
    else if (temperature > 25)
      cssClass = "hot";
    float hic = dht.computeHeatIndex(temperature, temperature2, false);
    float lpg = mq2.readLPG();
  
  float co = mq2.readCO();
  float rd=analogRead(pin2);
  float smoke = mq2.readSmoke();
    String message = "<!DOCTYPE html><html><head><title>" + title + "</title><meta charset=\"utf-8\" /><meta name=\"viewport\" content=\"width=device-width\" /><link href='https://fonts.googleapis.com/css?family=Advent+Pro' rel=\"stylesheet\" type=\"text/css\"><style>\n";
    message += "html {height: 100%;}";
    message += "div {color: #fff;font-family: 'Advent Pro';font-weight: 400;left: 50%;}";
    message += "h2 {font-size: 90px;font-weight: 400; margin: 0}";
    message += "body {height: 100%;}";
    message += ".cold {background: linear-gradient(to bottom, #7abcff, #0665e0 );}";
    message += ".mediumhot {background: linear-gradient(to bottom, #81ef85,#057003);}";
    message += ".hot {background: linear-gradient(to bottom, #fcdb88,#d32106);}";
    message += "</style></head><body class=\"" + cssClass + "\"><div><h1>" + title +  "</h1><h2>" + temperatureString + "&nbsp;<small>&deg;C</small></h2></div><br><br><br><div><h1>" + title2 +  "</h1><h2>" + rd + "%</h2></div><div><h1>" + title2 +  "</h1><h2>" + temperatureString2 + "%</h2></div></div><br><br><br><div><h1>" + title3 +  "</h1><h2>" + hic + "%</h2></div><div><h1>" + title4 +  "</h1><h2>" + lpg + "%</h2></div><div><h1>" + title5 +  "</h1><h2>" + co + "%</h2></div><div><h1>" + title6 +  "</h1><h2>" + smoke + "%</h2></div></body></html>";
    
    server.send(200, "text/html", message);
    if (temperature < 15)
   {
    digitalWrite(D0, HIGH);
    delay(3000);
   }
   if (temperature < 25)
   {
    digitalWrite(D0, HIGH);
    delay(5000);
   }
   if (temperature > 25)
   {
    digitalWrite(D0, HIGH);
    delay(7000);
   }
  });

  server.begin();
  
  Serial.println("Temperature web server started!");
}

void loop(void){
  server.handleClient();
}
