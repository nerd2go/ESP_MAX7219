
/****************************************************************************
 * Name: ESP_Max7219_Video_1                                                *
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

// Einbinden der Libraries für WiFi
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

// Deklarieren der Variablen
const char* ssid     = "xxx";
const char* password = "xxx";

int pinCS = D4; // "CS" ist mit diesem Pin verbunden 
//int pinCS = 5; // "CS" ist mit diesem Pin verbunden bei dem ESP32 DOIT DEVKIT V1
int numberOfHorizontalDisplays = 4; // Anzahl der Display Horizontal
int numberOfVerticalDisplays   = 1; // Anzahl der Display Vertikal
//                    123456789
char time_value[9] = "00:00:00"; // Hier wird die Zeit als einzelen Zeichen abgelegt

// Initialisieren ...
// ... des WiFiClient
WiFiClient client;
// ... der Matrix
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Projekt: ESP_MAX7219_Video_1");
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println();
  
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
  configTime(0 * 3600, 0, "ntp.uni-regensburg.de", "192.168.178.1");
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
  
  int start[5]= {2,8,14,20,26}; 
  
  // Hier werden 5 Werte abgelegt, in den Speicher start[0] bis start[4]
  // an dieser Stelle wird das Zeichen geschrieben
  //
  // Beispiel 23:15:47
  // 10er Stunden -> start[0] = 2  => time_value[0] = 2
  // Stunden      -> start[1] = 8  => time_value[1] = 3
  // Doppelpunkt  -> start[2] = 14 => time_value[2] = :
  // 10er Minuten -> start[3] = 20 => time_value[3] = 1
  // Minuten      -> start[4] = 26 => time_value[4] = 5
  //
  // Doppelpunkt                   => time_value[5] = :
  // 10er Sekunden                 => time_value[6] = 4
  // Sekunden                      => time_value[6] = 7
  //

  matrix.fillScreen(LOW);
  time_t now = time(nullptr);
  String time = String(ctime(&now));

  //Inhalt in Sting "time"
  //          1         2
  //012345678901234567890123
  //Tue Jul 27 22:01:52 2021
  
  //Inhalt in char* "time_value"
  //01234567
  //22:01:52 
  time.substring(11,19).toCharArray(time_value, 9); 

  for (int i=0;i<=4;i++){ 
    // Wenn i = 2 (Doppelpunkt) und die Sekunden Modulo 2 = 0 dann nichts anzeigen
    // Das heisst, dass bei jeder graden Sekunde der Doppelpunkt NICHT zu sehen ist.
    if(i==2 && (time_value[7]%2)==0){  
      matrix.drawChar(start[i],0, ' ' , HIGH,LOW,1);
    }
    else{
      matrix.drawChar(start[i],0, time_value[i], HIGH,LOW,1);
    }
  }
  matrix.write(); // Zeichen in das Display schreiben
  delay(1000);
}
