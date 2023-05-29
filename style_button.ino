#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"
#define DHT11PIN 16
#define RELAY_PIN 4


#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

/*Put your SSID & Password*/
const char* ssid = "Mauran";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

WebServer server(80);

// DHT Sensor
uint8_t DHTPin = 4; 
               
// Initialize DHT sensor.
DHT dht(DHT11PIN, DHT11);                

float humi = dht.readHumidity();
float temp = dht.readTemperature();
bool fanStatus = false;
String fan; 
void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(DHTPin, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  dht.begin();              

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/fan", handle_FanControl);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
 

}


void loop() {
  
  server.handleClient();
  float temp = dht.readTemperature();
 

  if (temp > 35 || !fanStatus) {
    Serial.println("FAN ON");
    digitalWrite(RELAY_PIN, LOW);  // turn on
    
  } else if (temp <= 35 || fanStatus) {
    Serial.println("FAN OFF");
    digitalWrite(RELAY_PIN, HIGH); // turn off
   
  }
  delay(2000); // Delay between temperature readings
}

void handle_FanControl() {
  fanStatus = !fanStatus; // Toggle the fan status
  if (fanStatus) {
    Serial.println("FAN ON");
    digitalWrite(RELAY_PIN, LOW);
  } else {
    Serial.println("FAN OFF");
    digitalWrite(RELAY_PIN, HIGH);
  }
  server.sendHeader("Location", "/", true); // Redirect to the main page after handling fan control
  server.send(302, "text/plain", "");
}

void handle_OnConnect() {

 float humi = dht.readHumidity();
 float temp = dht.readTemperature();
 //String fanStatus = (temp > 35) ? "ON" : "OFF";
 String fan = (temp > 35) ||  fanStatus ? "OFF" : "ON";
 //char fan ();// Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(temp,humi,fanStatus,fan)); 
}



void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}



String SendHTML(float temp,float humi, bool fanStatus,String fan ){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title> Weather Report</title>\n";
  ptr +="<style> html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;background-color: #FFC0CB}\n";
  ptr +="h1 {color: blue;font-size: 60px;}\n";
  ptr +="h3 {color: skyblue;font-size: 30px;}\n";
  ptr +="h2 {color: black;font-size: 45px;}\n";


 ptr +="button {background-color: #4CAF50;color: white;padding: 10px 20px;text-align: center;}\n";

  
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";

  ptr +="<div id=\"webpage\">\n";
  
   ptr +="<h1 > Weather Report </h1>\n";
  
   ptr +="</div>\n";
   ptr +="<h2 > Temperature :</h2> <h3 style='color:red'>";
   ptr +=(float)temp;
   ptr +="  Celsius</h3>\n";
  
   
   ptr +="<h2> Humidity :</h2> <h3> ";
   
   ptr +=(float)humi;
   ptr +="%</h3>\n";
   
   ptr += "<h2>Fan:</h2> <h3>";
  ptr += fan;
  ptr += "</h3>\n";

  ptr += "<h2>Fan:</h2>\n";
  ptr += "<form action=\"/fan\" method=\"POST\">\n";
  ptr += "<button type=\"submit\">";
  if (fanStatus) {
    ptr += "Turn ON";
  } else {
    ptr += "Turn OFF";
  }
  ptr += "</button>\n";
  ptr += "</form>\n";
 
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
