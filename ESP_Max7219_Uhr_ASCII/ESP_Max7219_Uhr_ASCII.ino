
/****************************************************************************
 * Name: ESP_Max7219_Uhr_Ascii                                                *
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
#include <Max72xxPanel.h>   // Nur auf GitHub: https://github.com/markruys/arduino-Max72xxPanel 
#include <time.h>

// Die Zeitstruktur 
struct tm tm; // https://www2.hs-fulda.de/~klingebiel/c-stdlib/time.htm

// Deklarieren der Variablen
const char* ssid     = "xxxx";
const char* password = "xxxx";

int pinCS = D4; // "CS" ist mit diesem Pin verbunden 
//int pinCS = 5; // "CS" ist mit diesem Pin verbunden bei dem ESP32 DOIT DEVKIT V1

// Angaben über die Matrix
int numberOfHorizontalDisplays = 4; // Anzahl der Displays Horizontal, ein "Block" von 8x8 Punkten ist ein Display
int numberOfVerticalDisplays   = 1; // Anzahl der Displays Vertikal

// Deklarationen für Ticker
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

// Initialisieren ...
// ... des WiFiClient
WiFiClient client;
// ... der Matrix
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setup() {
  Serial.begin(115200);
  delay(500);

  // Einstellen der ggf. benötigten Rotation
  matrix.setRotation(0, 1);    
  matrix.setRotation(1, 1);    
  matrix.setRotation(2, 1);    
  matrix.setRotation(3, 1);
  
  // Einstellen der Helligkeit, die Werte können zwischen 0(dunkel) und 15(sehr hell) liegen.
  matrix.setIntensity(0);   
  matrix.fillScreen(LOW); // Display "leeren"

  // Ausgaben der Basisinformationen zum Programm
  Serial.println();
  Serial.printf("\n\nSketchname: %s\nBuild: %s\t\tIDE: %d.%d.%d\n%s\n\n",
                (__FILE__), (__TIMESTAMP__), ARDUINO / 10000, ARDUINO % 10000 / 100, ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10, ESP.getFullVersion().c_str());

  // Mit dem WLAN verbinden
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  matrix.setCursor(0,0);
  matrix.print("WLAN");
  matrix.write();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  matrix.fillScreen(LOW);
  matrix.setCursor(0,0);matrix.print("Ready");matrix.write();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");Serial.println(WiFi.localIP());
  Serial.println("");
  delay(1000);
  
  // Konfigurieren der Zeitserver
  int zeittype = 1; // zeittype = 0 -> Echtzeit // zeittype = 1 -> Fakezeit
                    // Über Zeittype = 1 stellen wir die Uhr auf 23:59:45 ein. Damit die Zeit bei den Tests nicht zu lang wird.
  setenv("TZ", "GMT-1BST,M3.5.0/01,M10.5.0/02",1);
  if (zeittype==0){
    configTime(0 * 3600, 0, "de.pool.ntp.org", "ntp.uni-regensburg.de", "192.168.178.1");
  }
  else{
    time_t rtc = 1628114385; // 1628125185 -> 04.08.2021 23:59:45 UTC+2; // https://www.confirado.de/tools/timestamp-umrechner.html
    timeval tv = { rtc, 0 };
    settimeofday(&tv, nullptr);
  }
}
void loop()  {
  time_t now = time(&now);
  localtime_r(&now,&tm);
  String meine_zeit = String(ctime(&now)); // in dem String "meine_zeit" wird die aktuelle Zeit gespeichert
  /* Inhalt in String "meine_zeit"
              1         2
    012345678901234567890123
    Tue Jul 27 22:01:52 2021
  */
  static String letzte_zeit; // Zwischenspeicher für die Zeit
  //zeige_ascii(); // ASCII/Zeichensatz Tabelle im Netz: https://www.techstext.com/ascii-code-type/

  // Nur wenn sich die gespeicherte Sekunde von der aktuellen Sekunde unterscheidet
  if (letzte_zeit.substring(17,19)!=meine_zeit.substring(17,19)){ // https://www.arduino.cc/reference/de/language/variables/data-types/string/functions/substring/
     Serial.println(meine_zeit.substring(17,19));
     /*
     switch(letzte_zeit.substring(17,19).toInt()){
      case 10: 
                vScrollText(meine_zeit.substring(11,16) , "    ", 180);
                vScrollText("    ", "Dirk@",180);
                vScrollText("Dirk@", "N2Go",180);
                vScrollText("N2Go", "     ", 180);
                vScrollText("    ", meine_zeit.substring(11,16), 180);
                break;
      case 20:
                ticker("20", 80);
                break;
      case 30:
                ticker("30", 80);
                break;                
      case 40:
                ticker("40", 80);
                break;
      case 50:
                ticker("50", 80);
                break;
      default:
              zeige_dreh_uhr(meine_zeit,letzte_zeit,80);
    }
    */
    zeige_uhr(meine_zeit);
    letzte_zeit = meine_zeit;
  }
}
void zeige_dreh_uhr(String meine_zeit_org, String letzte_zeit_org, int wait){

  String meine_zeit = meine_zeit_org.substring(11,13) + meine_zeit_org.substring(14,16);
  String letzte_zeit = letzte_zeit_org.substring(11,13) + letzte_zeit_org.substring(14,16);
  /*
  String meine_zeit = meine_zeit_org.substring(14,16) + meine_zeit_org.substring(17,19);
  String letzte_zeit = letzte_zeit_org.substring(14,16) + letzte_zeit_org.substring(17,19);
  */  
  char trenner = ' ';
  int start[4] = {2,8,20,26};
  matrix.fillScreen(LOW);
  matrix.setCursor(2,0);matrix.print(meine_zeit.substring(0,2));
  matrix.setCursor(20,0);matrix.print(meine_zeit.substring(2,4));
  if((meine_zeit_org.substring(17,19).toInt()%2)==0){  
    trenner = ':';
  }
  matrix.drawChar(14,0,trenner, HIGH,LOW,1);
  for (int i=0;i<=8;i++){ // Zahl von oben einrollen.
    //scrollChar(x POS,aktuelle i(y), "alte" Zahl als char, neue Zahl als char);
    for (int t=0;t<=3;t++){
      if (meine_zeit[t]!=letzte_zeit[t]){
        scrollChar(start[t], i,letzte_zeit[t],meine_zeit[t]);
      }
    }
    matrix.write();
    delay(wait);
  }
}
void zeige_uhr(String meine_zeit){
  // Das blinkende Symbol definieren
    String trenner ;
    if (meine_zeit.substring(17,19).toInt()%2==0){
        trenner = ":";
        //trenner = char(174); // doppelpfeil links
        //trenner = char(9); // kreis
        
    }
    else{
        trenner = char(3);   // Herz
        //trenner = char(15);  // Sonne
        //trenner = char(4);   // Karo
        //trenner = char(175); // doppelpfeil rechts
        
    }
    // String zur Ausgabe zusammensetzen
    String zeit =  meine_zeit.substring(11,13) + trenner + meine_zeit.substring(14,16);
    matrix.fillScreen(LOW); // Matrix leeren
    matrix.setCursor(2,0);  // Courser positionieren
    matrix.print(zeit);     // Zeit ausgeben
    matrix.write();
}
void vScrollText(String text_alt, String text_neu, int wait){
  for (int i=8;i>=0;i--){
      matrix.fillScreen(LOW);
      matrix.setCursor(2,(i-matrix.height()))   ;matrix.print(text_alt);
      matrix.setCursor(2,i) ;matrix.print(text_neu);
      matrix.write();
      delay(wait);
  }
}
void scrollChar(int x, int y, char alt, char neu){
  matrix.drawChar(x,y,alt, HIGH,LOW,1);
  matrix.drawChar(x,(y-matrix.height()),neu, HIGH,LOW,1);
}
void zeige_ascii(void){
  for (int z=0;z<=255;z++){ // https://www.techstext.com/ascii-code-type/
    matrix.fillScreen(LOW);
    if (z<=9){
        matrix.setCursor(14,0);matrix.print(String(z)+ "=" );
    }
    else if (z<=99){
      matrix.setCursor(8,0);matrix.print(String(z)+ "=" );
    }
    else{
      matrix.setCursor(1,0);matrix.print(String(z)+ "=" );
    }
    matrix.drawChar(26,0,z, HIGH,LOW,1);
    matrix.write();
    Serial.print(String(z)+ "=" );Serial.write(z);Serial.println();
    yield(); // https://www.arduino.cc/en/Reference/SchedulerYield
    delay(1000);
  }
}
void ticker(String text, int wait){
  for ( int i = 0 ; i < width * text.length() + matrix.width() - 1 - spacer; i++ ) {
    matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;                                           
    int y = (matrix.height() - 8) / 2;
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < text.length() ) {
        matrix.drawChar(x, y, text[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write();
    delay(wait);
  }
}
