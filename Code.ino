#include <ESP8266WiFi.h>             // ESP8266 WeMos D1 Mini
#include <WiFiClient.h>              // Wifi Library
#include <ESP8266Ping.h>             // https://github.com/dancol90/ESP8266Ping/

const char* WIFI_SSID = "XXXX";      // Your SSID
const char* WIFI_PWD  = "XXXX";      // Your WiFi Password

#define LED_RED    D7
#define LED_GREEN  D8
#define LED_BLUE   D6

int ctr               = 0;
int brightness        = 260;         // How bright the LED is (0 = full, 512 = dim, 1023 = off)
int fadeAmount        = 20;          // How many points to fade the LED by
String direction      = "FO";        // Fade In "FI" Fade Out "FO"


void setup() {
  Serial.begin(9600);

  /** Define the pin modes **/
  pinMode(LED_RED,   OUTPUT); analogWrite(LED_RED,    1023);
  pinMode(LED_BLUE,  OUTPUT); analogWrite(LED_BLUE,   1023);
  pinMode(LED_GREEN, OUTPUT); digitalWrite(LED_GREEN, HIGH);
  
  /** We start by connecting to a WiFi network **/  
  IPAddress local_IP(192, 168, 0, 240);      // Static IP Address for ESP8266
  IPAddress subnet(255, 255, 255, 0);       // Subnet Mask
  IPAddress gateway(192, 168, 0, 1);        // Default Gateway

  /*********** Setup a WiFi connection ***********/
  Serial.println(); 
  if (WiFi.config(local_IP, gateway, subnet)) { Serial.println("Static IP Configured"); }
  else                                        { Serial.println("Static IP Configuration Failed"); };

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    switch(ctr){  // Green LED Flashes On Boot
      case 0: digitalWrite(LED_GREEN, HIGH); ctr=1; break;
      case 1: digitalWrite(LED_GREEN, LOW);  ctr=0; break;
    };
  };
  Serial.println("\nWiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  digitalWrite(LED_GREEN, HIGH); // Turn off the Green LED
};


void loop() {
  if (pingTest()) BrightnessController("BLUE"); // If a successful ping is received    
  else            BrightnessController("RED");  // If the ping fails turn on the red LED

  if (millis() >= 3600000) ESP.reset();         // Reboot the WEMOS D1 Mini Every Hour = 3600000 milliseconds
};


/** Function that controls the LED Brightness **/
void BrightnessController( String LED ){
  if (direction == "FI") { brightness = brightness + fadeAmount; };
  if (direction == "FO") { brightness = brightness - fadeAmount; };

  if (brightness < 0)   { brightness = 0   + fadeAmount; direction = "FI";}
  if (brightness > 260) { brightness = 280 - fadeAmount; direction = "FO";}

  //delay(20);         // Pause so you can see each brightness level
  
  if (LED == "RED")  { analogWrite(LED_RED,  brightness); };
  if (LED == "BLUE") { analogWrite(LED_BLUE, brightness); };

  Serial.print("LED: "); Serial.println(LED);
  Serial.print("Direction: "); Serial.println(direction);
  Serial.print("Brightness: "); Serial.println(brightness);
};


/** Function that performs the ping test **/
bool pingTest(){
  IPAddress ip (8, 8, 8, 8);           // The remote ip to ping
  Serial.print("Pinging host : "); Serial.println(ip);
  if(Ping.ping(ip, 2)) {               // Ping the remote host with two ping requests
    Serial.println("Success!!!!!!!");
    analogWrite(LED_RED,   1023);      // Turn off the red LED
    return true;
  } else {
    Serial.println("Error :(");
    analogWrite(LED_BLUE,   1023);     // Turn off the blue LED
    return false;
  };
};
