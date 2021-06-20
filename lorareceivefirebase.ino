#include "heltec.h"
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6

#include "time.h"
#include <IOXhop_FirebaseESP32.h>
#define FIREBASE_HOST "incidentresponse.firebaseio.com"   
#define FIREBASE_AUTH "yQ97r6iwL2VTOVuPu6Fjvhq3BQOmo752QzMPDy4P"   
#define WIFI_SSID "Emergency Portal"               
#define WIFI_PASSWORD ""

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

String time11;
char data1;
String printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  char timeStringBuff[50]; //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  //print like "const char*"
  Serial.println(timeStringBuff);

  return timeStringBuff;
}

void setup() {
    //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
Serial.begin(115200);
  delay(1000);            
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Heltec.display->flipScreenVertically();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "wifi connected");
Heltec.display->display();
delay(1000);
Heltec.display->clear();
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
Firebase.setString("/","");
   Heltec.display->drawString(0, 10, "i am ready");
   Heltec.display->display();
   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}
int count=0;
void addtofirebase(String dat)
{
  count+=1;
  Firebase.setString("/"+String(count),dat);
  Serial.println("added data");
}
void loop() {
  // try to parse packet
  //addtofirebase("Sumukh,9148939841,12.365,75.123,Flood,Rescue,Electricity,10,Helzp");
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");
    // read packet
    String dat="";
    while (LoRa.available()) {
      dat+=((char)LoRa.read());
    }
    Heltec.display->clear();
    Heltec.display->drawString(0, 10,dat);
   Heltec.display->display();
    
    Serial.print(dat);
    addtofirebase(dat);
    // print RSSI of packet
    Serial.println(LoRa.packetRssi());
    delay(5000);
    Heltec.display->clear();
   Heltec.display->drawString(0, 10, "i am ready");
   Heltec.display->display();
  }
}
