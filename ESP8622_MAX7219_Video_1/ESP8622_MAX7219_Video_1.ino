
/****************************************************************************
 * Name: ESP_Max7219_Video_1					                            *
 *                                                                          *
 * Anschlussplan:                                                           *  
 *                                                                          *
 *  LED Matrix Pin -> ESP8266 Pin NodeMCU // ESP32 DOIT Devkit V1           *
 *                    oder WeMos D1                                         *
 *  Vcc            -> 3v3                 // VCC  -> 3v3                    *
 *  Gnd            -> Gnd                 // GND  -> GND                    *
 *  DIN            -> D7                  // DIN  -> G23                    * 
 *  CS             -> D4                  // CS   -> G5                     *
 *  CLK            -> D5                  // CLK  -> G18                    *
 *                                                                          *
 * Erstellt: dirk@n2go.de                                                   *
 *                                                                          *
 ****************************************************************************/

// Einbinden der Libraries für WiFi und DNS 
#include <DNSServer.h>      // wenn die Zeitserver als "Name" hinterlegt werden, bei IP kann es weg bleiben.

#if defined(ESP8266) 
  #include <ESP8266WiFi.h>  // WiFi für ESP12
#elif defined(ESP32) 
  #include <WiFi.h>           // WiFi für ESP32
#else 
  #error "Kein ESP8266 oder ESP32 vorhanden." 
#endif  

// Einbinden der Libraries (ESP unabhänig)
#include <Adafruit_GFX.h>   // In der Bibliothek zu finden oder unter:https://github.com/adafruit/Adafruit-GFX-Library
#include <Max72xxPanel.h>   // Nur auf Git: https://github.com/markruys/arduino-Max72xxPanel 
#include <time.h>

// Definieren der Variablen
const unsigned int BAUDRATE       =  115200;       // Baudrate für den seriellen Monitor

const char* ssid     = "xxx";
const char* password = "xxx";

int pinCS = D4; // "CS" ist mit diesem Pin verbunden 
//int pinCS = 5; // "CS" ist mit diesem Pin verbunden bei dem ESP32 DOIT DEVKIT V1
int numberOfHorizontalDisplays = 4; // Anzahl der Display Horizontal
int numberOfVerticalDisplays   = 1; // Anzahl der Display Vertikal
char time_value[20]; // Hier wird die Zeit als einzelen Zeichen abgelegt

int spacer = 1; // Leerstellen zwischen den Zeichen
int buchstaben_breite  = 5 + spacer; // Die Breite der Schrift + den Zwischenraum (1 Pixel)

// Initialisieren ...
// ... des WiFiClient
WiFiClient client;
// ... der Matrix
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println("\nProjekt: ESP_Max7219_Video_1");

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  delay(1000);
  // Konfigurieren der Zeitserver
  configTime(0 * 3600, 0, "192.168.1.1", "192.168.178.1");
  setenv("TZ", "GMT-1BST,M3.5.0/01,M10.5.0/02",1);

  // Einstellen der ggf. benötigetn Rotation
  matrix.setRotation(0, 1);    
  matrix.setRotation(1, 1);    
  matrix.setRotation(2, 1);    
  matrix.setRotation(3, 1);
  
  // Einstellen der Helligkeit, die Werte können zwischen 0(dunkel) und 15(sehr hell) liegen.
  matrix.setIntensity(0); 
}

void loop() {
  // Hier werden 5 Werte abgelegt, in den Speicher start[0] bis start[4]
  // an dieser Stelle wird das Zeichen geschrieben
  // 10er Stunden -> start[0] = 2
  // Stunden      -> start[1] = 8
  // Doppelpunkt  -> start[2] = 14
  // 10er Minuten -> start[3] = 20
  // Minuten      -> start[4] = 26
  //
  int start[5]= {2,8,14,20,26}; 

  matrix.fillScreen(LOW);
  time_t now = time(nullptr);
  String time = String(ctime(&now));
  time.trim();
  time.substring(11,19).toCharArray(time_value, 10); 

  for (int i=0;i<=4;i++){  
    matrix.drawChar(start[i],0, time_value[i], HIGH,LOW,1);
  }
 
  matrix.write(); // Die Zeichen anzeigen
  delay(1000);
}
