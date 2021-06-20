#include <Arduino.h>
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "heltec.h"
#define BAND    433E6
AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "Emergency Portal";
const char* password = "";

const char* PARAM_NAME = "inputName";
const char* PARAM_ADDR = "inputAddr";
const char* PARAM_EMERGENCY = "inputEmergency";
const char* PARAM_ITEM = "inputItem";
const char* PARAM_HAZARD = "inputHazard";
const char* PARAM_PEOPLE = "inputPeople";
const char* PARAM_ALONE = "inputAlone";
const char* PARAM_MSG = "inputMsg";


// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Emergency Portal</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
<br>
<h1><strong> <center>Emergency Network Portal</h1></strong></center>
<br>
<p> <center>Click 'Submit' after filling up the fields<p></center>
<br><br>
<form action="/get" target="hidden-form">
<b>Enter your name</b><br>
<input type="text" name="inputName">
<input type="submit" value="Submit" onclick="submitMessage()">
</form><br>

<form action="/get" target="hidden-form">
<b>Enter your address</b><br>
<input type="text" name="inputAddr">
<input type="submit" value="Submit" onclick="submitMessage()">
</form><br>
<br>
<form action="/get" target="hidden-form">
<fieldset>      
        <legend><b>What is Your Emergency?</b></legend><br>   
        <input type="radio" name="emr1" value="Medical">Medical<br>      
        <input type="radio" name="emr1" value="Violence">Violence<br>      
        <input type="radio" name="emr1" value="Fire">Fire<br>
        <input type="radio" name="emr1" value="Flood">Flood<br>      
        <input type="radio" name="emr1" value="Landslide">Landslide<br>      
        <input type="radio" name="emr1" value="Explosion">Explosion<br>      
        <br>      
        <input type="submit" value="Submit" onclick="submitMessage()">    
</fieldset>      
</form>
<br>
<form action="/get" target="hidden-form">
<fieldset>      
        <legend><b>What do you need in 24Hrs?</b></legend><br>   
        <input type="radio" name="emr2" value="First aid">First aid<br>      
        <input type="radio" name="emr2" value="Shelter">Shelter<br>      
        <input type="radio" name="emr2" value="Food">Food<br>
        <input type="radio" name="emr2" value="Financial">Financial aid<br>      
        <input type="radio" name="emr2" value="Water">Water<br>      
        <input type="radio" name="emr2" value="Evacuation">Evacuation<br>
        <input type="radio" name="emr2" value="Clothing">Clothing<br>        
        <br>      
        <input type="submit" value="Submit" onclick="submitMessage()">    
</fieldset>      
</form>
<br>
<form action="/get" target="hidden-form">
<fieldset>      
        <legend><b>What hazards are currently near you?</b></legend><br>   
        <input type="radio" name="emr3" value="Electrical">Electrical<br>      
        <input type="radio" name="emr3" value="road">Road blocked<br>      
        <input type="radio" name="emr3" value="Fallen Trees">Fallen Trees<br>
        <input type="radio" name="emr3" value="Chemical spill">Chemical spill aid<br>      
        <input type="radio" name="emr3" value="Smoke/fire">Smoke/fire<br>      
        <input type="radio" name="emr3" value="Fast current">Fast current<br>
        <input type="radio" name="emr3" value="Explosion">Explosion<br>        
        <br>      
        <input type="submit" value="Submit" onclick="submitMessage()">    
</fieldset>      
</form>
<br>
<form action="/get" target="hidden-form">
<fieldset>      
        <legend><b>Are you alone or in a group?</b></legend><br> 
        <input type="radio" name="alon" value="Alone">Alone<br>      
        <input type="radio" name="alon" value="Group">In a group<br> 
        <input type="submit" value="Submit" onclick="submitMessage()">
</fieldset>
</form><br>
<br>
<br>

<form action="/get" target="hidden-form">
<b>Message</b><br>
<input type="text" name="inputMsg">
<input type="submit" value="Submit" onclick="submitMessage()">
</form><br>

  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
 String strr="this is: ";
 for (int i=0;i<10;i++)
 strr+=message[i];
  Serial.print(strr);
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "inputName"){
    return readFile(SPIFFS, "/inputName.txt");
  }
  else if(var == "inputAddr"){
    return readFile(SPIFFS, "/inputAddr.txt");
  }
  else if(var == "inputEmergency"){
    return readFile(SPIFFS, "/inputEmergency.txt");
  }
  else if(var == "inputItem"){
    return readFile(SPIFFS, "/inputItem.txt");
  }
  else if(var == "inputHazard"){
    return readFile(SPIFFS, "/inputHazard.txt");
  }
  else if(var == "inputPeople"){
    return readFile(SPIFFS, "/inputPeople.txt");
  }
  else if(var == "inputAlone"){
    return readFile(SPIFFS, "/inputAlone.txt");
  }
  else if(var == "inputMsg"){
    return readFile(SPIFFS, "/inputMsg.txt");
  }
  return String();
}
String st="";
int count=0;
void setup() {
  Serial.begin(115200);
  // Initialize SPIFFS
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname("emergency");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Heltec.display->flipScreenVertically();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "Portal Online");
  Heltec.display->drawString(0, 30,WiFi.localIP().toString());
Heltec.display->display();
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputName=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_NAME)) {
      inputMessage = request->getParam(PARAM_NAME)->value();
      
    }
else if (request->hasParam(PARAM_ADDR)) {
      inputMessage = request->getParam(PARAM_ADDR)->value()+",12.32001,76.60502";      
    }
else if (request->hasParam("emr1")) {
      inputMessage = request->getParam("emr1")->value();
    }

    else if (request->hasParam("emr2")) {
      inputMessage = request->getParam("emr2")->value();
    }
else if (request->hasParam("emr3")) {
      inputMessage = request->getParam("emr3")->value()+",20";
    }
    else if (request->hasParam(PARAM_PEOPLE)) {
      inputMessage = request->getParam(PARAM_PEOPLE)->value();
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam("alon")) {
      inputMessage = request->getParam("alon")->value();
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    else if (request->hasParam(PARAM_MSG)) {
      inputMessage = request->getParam(PARAM_MSG)->value();
    }
    else {
      inputMessage = "";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
    if (inputMessage!=""){
      count+=1;
      st+=inputMessage+",";
    }
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if (count==7){
    count=0;
    LoRa.beginPacket();
    
    LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print(st);
  LoRa.endPacket();
  Serial.println("sent");
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "Data Sent");
  delay(5000);
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "Portal Online");
  Heltec.display->drawString(0, 30,WiFi.localIP().toString());
Heltec.display->display();
st="";

  }
  delay(50);
}
