/*
 * 
 * ESP Robi - Wireless controlled talking robot 
 * Mehanics by Mario Pavlić
 * ESP code by Goran Mahovlić
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

//Sound module library
#include <Wtv020sd16p.h> 

//Sound Module pins
#define resetPin 14  // The pin number of the reset pin.
#define clockPin 12  // The pin number of the clock pin.
#define dataPin 13  // The pin number of the data pin.
#define busyPin 5  // The pin number of the busy pin.

//Motor driver pins
#define PWM1 15
#define PWM2 0
#define PWM3 2
#define PWM4 4

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "otvoreno";

/*
Create an instance of the Wtv020sd16p class.
1st parameter: Reset pin number.
2nd parameter: Clock pin number.
3rd parameter: Data pin number.
4th parameter: Busy pin number.
*/

Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

// Use this if you want to connect roby to your wireless
// wifi connection variables
// const char* ssid = "YourRouterSSID";
//  const char* password = "YourRouterPassword";

    boolean wifiConnected = false;

    // UDP variables - we will use port 8888 for communications
    unsigned int localPort = 8888;
    WiFiUDP UDP;
    boolean udpConnected = false;
    char ReplyBuffer[] = "acknowledged"; // a string to send back

    void setup() {
    // Initialise Serial connection
    Serial.begin(115200);
    // Initialise wifi connection
   
  // wifiConnected = connectWifi(); // enable for connectiong to your AP
   setupWiFi();

    // only proceed if wifi connection successful
    if(wifiConnected){
    udpConnected = connectUDP();
    if (udpConnected){
      // Start sound module
    wtv020sd16p.reset();
      // Set all motor pins to LOW
    pinMode(PWM1,OUTPUT);
    digitalWrite(PWM1,LOW);
    pinMode(PWM2,OUTPUT);
    digitalWrite(PWM2,LOW);    
    pinMode(PWM3,OUTPUT);
    digitalWrite(PWM3,LOW);    
    pinMode(PWM4,OUTPUT);
    digitalWrite(PWM4,LOW);    
    }
    }
    }

    void loop() {
    // check if the WiFi and UDP connections were successful
    if(wifiConnected){
    if(udpConnected){

    // if there’s data available, read a packet
    int packetSize = UDP.parsePacket();
    if(packetSize)
    {
    Serial.println("");
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = UDP.remoteIP();
    for (int i =0; i < 4; i++)
    {
    Serial.print(remote[i], DEC);
    if (i < 3)
    {
    Serial.print(".");
    }
    }
    Serial.print(", port ");
    Serial.println(UDP.remotePort());
    char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
    // read the packet into packetBufffer
    UDP.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    int songNo = atoi(packetBuffer);
    Serial.println(packetBuffer);
    Serial.println(packetBuffer[0]); 
    if(packetBuffer[0] == 'L'){
      Serial.println("Left");
      motorAllStop();
      motorLeftFW();
      motorRightBW();
    }
    else if(packetBuffer[0] == 'R'){
      Serial.println("Right");
      motorAllStop();
      motorLeftBW();
      motorRightFW();
    }
    else if(packetBuffer[0] == 'F'){
      Serial.println("Forward");
      motorAllStop();
      motorLeftFW();
      motorRightFW();
    }
    else if(packetBuffer[0] == 'B'){
      Serial.println("Backward");
            motorAllStop();
            motorLeftBW();
            motorRightBW();
    }        
    else if(packetBuffer[0] == 'P'){
      Serial.println("STOP");
            motorAllStop();
    }
    else if (songNo>=0 && songNo<8){
      wtv020sd16p.stopVoice();
      wtv020sd16p.asyncPlayVoice(songNo);
      Serial.print("Playing song:");
      Serial.println(packetBuffer);
    }
    else if(songNo == 9)
    {
      wtv020sd16p.stopVoice();
    }
    int value = packetBuffer[0]*256 + packetBuffer[1];
    Serial.println(value);
    value = packetBuffer[0];
    Serial.println(value);
// send a reply, to the IP address and port that sent us the packet we received
// enable this if you want replay
//   UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
//   UDP.write(ReplyBuffer);
//   UDP.endPacket();
    }
    }
    }
    }

    // connect to UDP – returns true if successful or false if not
    boolean connectUDP(){
    boolean state = false;

    Serial.println("");
    Serial.println("Connecting to UDP");

    if(UDP.begin(localPort) == 1){
    Serial.println("Connection successful");
    state = true;
    }
    else{
    Serial.println("Connection failed");
    }

    return state;
    }


void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "Robi " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  delay(1000);
  wifiConnected = true;
}

void motorLeftFW(){
digitalWrite(PWM1,LOW);
digitalWrite(PWM2,HIGH);
}
void motorRightFW(){
digitalWrite(PWM3,HIGH);
digitalWrite(PWM4,LOW);
}
void motorLeftBW(){
digitalWrite(PWM1,HIGH);
digitalWrite(PWM2,LOW);
}
void motorRightBW(){
digitalWrite(PWM3,LOW);
digitalWrite(PWM4,HIGH);
}
void motorLeftStop(){
digitalWrite(PWM1,LOW);
digitalWrite(PWM2,LOW);
}
void motorRightStop(){
digitalWrite(PWM3,LOW);
digitalWrite(PWM4,LOW);
}
void motorAllStop()
{
digitalWrite(PWM1,LOW);
digitalWrite(PWM2,LOW);
digitalWrite(PWM3,LOW);
digitalWrite(PWM4,LOW);
}   

// connect to wifi – returns true if successful or false if not used to connect to local AP
/*
    boolean connectWifi(){
    boolean state = true;
    int i = 0;
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("Connecting to WiFi");

    // Wait for connection
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
    state = false;
    break;
    }
    i++;
    }
    if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    }
    else {
    Serial.println("");
    Serial.println("Connection failed.");
    }
    return state;
    }
    */
