/************************mq2sensor************************************/
/************************Hardware Related Macros************************************/
#define         MQ2PIN                       (0)     //define which analog input channel you are going to use
#define         RL_VALUE_MQ2                 (1)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR_MQ2      (9.577)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
                                                     //normal operation

/**********************Application Related Macros**********************************/
#define         GAS_HYDROGEN                  (0)
#define         GAS_LPG                       (1)
#define         GAS_METHANE                   (2)
#define         GAS_CARBON_MONOXIDE           (3)
#define         GAS_ALCOHOL                   (4)
#define         GAS_SMOKE                     (5)
#define         GAS_PROPANE                   (6)
#define         accuracy                      (0)   //for linearcurves
//#define         accuracy                    (1)   //for nonlinearcurves, un comment this line and comment the above line if calculations 
                                                    //are to be done using non linear curve equations
/*****************************Globals************************************************/
float           Ro = 0;  
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>    //DHT11 DHT22 

const char* ssid = "ok";
const char* password = "okokokok";

ESP8266WebServer server(80);

DHT dht(D4, DHT11);   // intialize our DHT11
;
char temperatureString[6];
char temperatureString2[6];
static char celsiusTemp[7];

void setup(void){
  Serial.begin(115200);   //Serial Communication Begin
  delay(10);
  dht.begin();   // dht monitoring
  
  WiFi.begin(ssid, password); //Conecting it to WiFi
  Serial.println(""); // Message that device is Connected or Not

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Ro = MQCalibration(MQ2PIN); 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  server.begin();      //ES8266 Webserver
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(1000);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", []() {
    float temperature = dht.readTemperature();
    dtostrf(temperature, 2, 2, temperatureString);
    float temperature2 = dht.readHumidity();
    dtostrf(temperature2, 2, 2, temperatureString2);
   
   float h2=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_HYDROGEN/10) ;
   float lpg=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_LPG/10) ;
 
   float ch4=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_METHANE/10) ;
    
   float co=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_CARBON_MONOXIDE) ;
    
   float alcohol=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_ALCOHOL/10) ;
 
   float smoke=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_SMOKE/10) ;
   
   float propane=MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_PROPANE/10);
   
   delay(200);
    String title = "Temperature";
    String title2 = "Humidity";
    String title3 = "Heat Index";
    
    String cssClass = "mediumhot";
    if (temperature < 15)
      cssClass = "cold";
    else if (temperature > 25)
      cssClass = "hot";
    float hic = dht.computeHeatIndex(temperature, temperature2, false);
    String message = "<!DOCTYPE html><html><head><title>" + title + "</title><meta charset=\"utf-8\" /><meta name=\"viewport\" content=\"width=device-width\" /><link href='https://fonts.googleapis.com/css?family=Advent+Pro' rel=\"stylesheet\" type=\"text/css\"><style>\n";
    message += "html {height: 100%;}";
    message += "div {color: #fff;font-family: 'Advent Pro';font-weight: 400;left: 50%;}";
    message += "h2 {font-size: 90px;font-weight: 400; margin: 0}";
    message += "body {height: 100%;}";
    message += ".cold {background: linear-gradient(to bottom, #7abcff, #0665e0 );}";
    message += ".mediumhot {background: linear-gradient(to bottom, #81ef85,#057003);}";
    message += ".hot {background: linear-gradient(to bottom, #fcdb88,#d32106);}";
    message += "</style></head><body class=\"" + cssClass + "\"><div><h1>" + title +  "</h1><h2>" + temperatureString + "&nbsp;<small>&deg;C</small></h2></div><br><div><h1>" + title2 +  "</h1><h2>" + temperatureString2 + "%</h2></div></div><br><br><br><div><h1>" + title3 +  "</h1><h2>" + hic + "%</h2></div><br><br><div><h1>PROPANE LEVEL</h1><h2>"+propane+"ppm</h2></div><div><h1>CO LEVEL</h1><h2>"+smoke+"ppm</h2></div><div><h1>ALCHOHOL LEVEL</h1><h2>"+alcohol+"ppm</h2></div><div><h1>CO2 LEVEL</h1><h2>"+co+"ppm</h2></div><div><h1>METHANE LEVEL</h1><h2>"+ch4+"ppm</h2></div><div><h1>LPG LEVEL</h1><h2>"+lpg+"ppm</h2></div><div><h1>HYDROGEN LEVEL</h1><h2>"+h2+"ppm</h2></div></body></html>";
    
    server.send(200, "text/html", message);
    if (temperature < 15)
   {
    digitalWrite(D0, HIGH);
    delay(300);
   }
   if (temperature < 25)
   {
    digitalWrite(D0, HIGH);
    delay(500);
   }
   if (temperature > 25)
   {
    digitalWrite(D0, HIGH);
    delay(700);
   }
  });

  server.begin();
  
  Serial.println("Temperature web server started!");
}

void loop(void){
  server.handleClient();

}
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE_MQ2*(1023-raw_adc)/raw_adc));
}

/***************************** MQCalibration ****************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use  
         MQResistanceCalculation to calculates the sensor resistance in clean air 
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs slightly between different sensors.
************************************************************************************/ 
float MQCalibration(int mq_pin)
{
  int i;
  float RS_AIR_val=0,r0;

  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {                     //take multiple samples
    RS_AIR_val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  RS_AIR_val = RS_AIR_val/CALIBARAION_SAMPLE_TIMES;              //calculate the average value

  r0 = RS_AIR_val/RO_CLEAN_AIR_FACTOR_MQ2;                      //RS_AIR_val divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                                 //according to the chart in the datasheet 

  return r0; 
}

/*****************************  MQRead *********************************************
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/ 
float MQRead(int mq_pin)
{
  int i;
  float rs=0;

  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs/READ_SAMPLE_TIMES;

  return rs;  
}

/*****************************  MQGetGasPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function uses different equations representing curves of each gas to 
         calculate the ppm (parts per million) of the target gas.
************************************************************************************/ 
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{ 
  if ( accuracy == 0 ) {
  if ( gas_id == GAS_HYDROGEN ) {
    return (pow(10,((-2.109*(log10(rs_ro_ratio))) + 2.983)));
  } else if ( gas_id == GAS_LPG ) {
    return (pow(10,((-2.123*(log10(rs_ro_ratio))) + 2.758)));
  } else if ( gas_id == GAS_METHANE ) {
    return (pow(10,((-2.622*(log10(rs_ro_ratio))) + 3.635)));
  } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
    return (pow(10,((-2.955*(log10(rs_ro_ratio))) + 4.457)));
  } else if ( gas_id == GAS_ALCOHOL ) {
    return (pow(10,((-2.692*(log10(rs_ro_ratio))) + 3.545)));
  } else if ( gas_id == GAS_SMOKE ) {
    return (pow(10,((-2.331*(log10(rs_ro_ratio))) + 3.596)));
  } else if ( gas_id == GAS_PROPANE ) {
    return (pow(10,((-2.174*(log10(rs_ro_ratio))) + 2.799)));
  }    
} 

  else if ( accuracy == 1 ) {
    if ( gas_id == GAS_HYDROGEN ) {
    return (pow(10,((-2.109*(log10(rs_ro_ratio))) + 2.983)));
  } else if ( gas_id == GAS_LPG ) {
    return (pow(10,((-2.123*(log10(rs_ro_ratio))) + 2.758)));
  } else if ( gas_id == GAS_METHANE ) {
    return (pow(10,((-2.622*(log10(rs_ro_ratio))) + 3.635)));
  } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
    return (pow(10,((-2.955*(log10(rs_ro_ratio))) + 4.457)));
  } else if ( gas_id == GAS_ALCOHOL ) {
    return (pow(10,((-2.692*(log10(rs_ro_ratio))) + 3.545)));
  } else if ( gas_id == GAS_SMOKE ) {
    return (pow(10,(-0.976*pow((log10(rs_ro_ratio)), 2) - 2.018*(log10(rs_ro_ratio)) + 3.617)));
  } else if ( gas_id == GAS_PROPANE ) {
    return (pow(10,((-2.174*(log10(rs_ro_ratio))) + 2.799)));
  }
}    
  return 0;
}
