#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>   // **v2.3.0** https://github.com/knolleary/pubsubclient

// PWM linear fade curve https://jared.geek.nz/2013/feb/linear-led-pwm
const unsigned char LinearLED[101] = {       
  0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 
  3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 
  8, 8, 9, 10, 10, 11, 12, 13, 14, 15, 
  16, 17, 18, 19, 20, 22, 23, 24, 26, 27, 
  29, 30, 32, 34, 35, 37, 39, 41, 43, 45, 
  47, 49, 51, 54, 56, 58, 61, 64, 66, 69, 
  72, 75, 78, 81, 84, 87, 90, 93, 97, 100, 
  104, 108, 111, 115, 119, 123, 127, 131, 136, 140, 
  145, 149, 154, 159, 163, 168, 173, 179, 184, 189, 
  195, 200, 206, 212, 217, 223, 230, 236, 242, 248, 
  255, 
};

// Constants
const char *wifi_ssid      = "SSID";
const char *wifi_pwd       = "WIFI PWD";
const char *wifi_hostname  = "LedDimmer";
const char* mqtt_server    = "192.168.1.189"; 		//MQTT Server IP, your home MQTT server eg Mosquitto on RPi, or some public MQTT
const int mqtt_port        = 1883; 					//MQTT Server PORT, default is 1883 but can be anything.
const char *mqtt_user      = "USER"; 		       	//User name in the Broker 
const char *mqtt_pwd       = "PWD";          		//Password to the Broker 
String clientId            = "LedDimmer : " + String(ESP.getChipId(), HEX);
const char *ota_hostname   = "LedDimmer";


// MQTT Constants

const char* mqtt_dimLightStatus_set_topic       = "LedDimmer/setLightStatus";
const char* mqtt_dimLightBrightness_set_topic   = "LedDimmer/setBrightness";
const char* mqtt_dimLightStatus_get_topic       = "LedDimmer/getLightStatus";
const char* mqtt_dimLightBrightness_get_topic   = "LedDimmer/getBrightness";


// PIN Setup
#define PIN_LED D5
#define PIN_BTN D3


// Global
int currentBrightness      = 0;     // LED OFF (0), ON (1-100) 
int lastBrightness         = 0;     // Last stored brightness, used fo next on
bool state                 = false; // LED OFF (false), ON (true) 


bool boolButtonPressed     = false;
bool duringLongClick       = false;
bool dimDirection          = true;  // true: dim up, false: dim down
byte debounceDuration      = 60;    // ms
word longClickDuration     = 400;   // ms
unsigned long lastMillis   = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  
  WiFi.hostname(wifi_hostname);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pwd);

  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.localIP());

}

void setup_ota() {

  // Set OTA Password, and change it in platformio.ini
  ArduinoOTA.setHostname(ota_hostname); 
  //ArduinoOTA.setPassword("ESP8266_PASSWORD");
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR);          // Auth failed
    else if (error == OTA_BEGIN_ERROR);    // Begin failed
    else if (error == OTA_CONNECT_ERROR);  // Connect failed
    else if (error == OTA_RECEIVE_ERROR);  // Receive failed
    else if (error == OTA_END_ERROR);      // End failed
  });
  ArduinoOTA.begin();

}

void reconnect() {

  // Loop until we're reconnected
  // Attempt to connect

    if (!client.connected()) {
      client.setServer(mqtt_server, mqtt_port);
      Serial.print("Connecting to MQTT server "); Serial.print(mqtt_server);

      while (!client.connected()) {
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd ))
        {
          Serial.println("\tconnected");
        } else {
          delay(500); 
          Serial.print(".");
        }
      }

      // Publish status
      PublishStatus();
      // ... and resubscribe
      client.subscribe(mqtt_dimLightStatus_get_topic);
      client.subscribe(mqtt_dimLightBrightness_get_topic);
 
   }
    
}

void PublishStatus(){

    // Publish Led Light status to MQTT broker

    char stateChar[5];  
    String stateString = String(state);
    stateString.toCharArray(stateChar, stateString.length()+1);
    client.publish(mqtt_dimLightStatus_set_topic, stateChar, true);
    
    char brightnessChar[5];
    String brightnessString = String(currentBrightness*0.01);
    brightnessString.toCharArray(brightnessChar, brightnessString.length()+1);
    client.publish(mqtt_dimLightBrightness_set_topic, brightnessChar, true);
}


void callback(char* topic, byte* payload, unsigned int length) {

    char c_payload[length];
    memcpy(c_payload, payload, length);
    c_payload[length] = '\0';

    String s_topic = String(topic);
    String s_payload = String(c_payload);

  // Handling incoming messages

    if ( s_topic == mqtt_dimLightStatus_get_topic ) {
     
      if (s_payload == "1") {
  
        if (state != true) {

          // Turn ON function will set last known brightness
          state = true;
          blink();

          if (lastBrightness == 0) { 
              setBrightness(100); 
              currentBrightness = 100; 
              }
          if (lastBrightness > 0) { 
              setBrightness(lastBrightness); 
              currentBrightness = lastBrightness; 
              }
         // PublishStatus();
        }

      } else if (s_payload == "0") {
              
        if (state != false) {

          // Turn OFF function and store last brightness
          state = false;
          blink();
          lastBrightness = currentBrightness; //Store brightness level
          setBrightness(0);
          currentBrightness = 0;
        }

      }

    } else if ( s_topic == mqtt_dimLightBrightness_get_topic ) {
      
      int receivedBrightness = s_payload.toFloat()*100;  

         //Store brightness level
        blink();
        setBrightness(receivedBrightness);
        currentBrightness = receivedBrightness;
      }
   // PublishStatus();
} 


void brightnessButton(){
    unsigned long currentMillis = millis();
    bool buttonState = !digitalRead(PIN_BTN); // Active low

    //button pressed
    if(!boolButtonPressed && buttonState){ 
        boolButtonPressed = true;
        lastMillis = currentMillis;
    }

    //button released
    if(boolButtonPressed && !buttonState){
        boolButtonPressed = false;
   
        if((currentMillis - lastMillis >= debounceDuration) &&        // Short click detected
           (currentMillis - lastMillis < longClickDuration)){ 
            
           if (currentBrightness == 0 && lastBrightness == 0){        // Max dim level if no last brightness stored
              setBrightness(100);                                   
              currentBrightness = 100;
              dimDirection = false;
              state = true;
              PublishStatus();                         
  
           } else if (currentBrightness == 0 && lastBrightness > 0){   // Turn on with last stored brightness       
              setBrightness(lastBrightness);
              currentBrightness = lastBrightness;
              state = true;
              PublishStatus();                      

           } else if (currentBrightness > 0){                           // Turn light off and store brightness  
              lastBrightness = currentBrightness;                                    
              setBrightness(0);
              currentBrightness = 0;
              dimDirection = true;
              state = false;
              PublishStatus();
              }
         }
    }

    // button is still pressed and it is a first long press detection
    if(boolButtonPressed && !duringLongClick && (currentMillis - lastMillis >= longClickDuration)){
              
        if(dimDirection){  // dim up
            dimDirection = false;
            int i = currentBrightness;
            while(!digitalRead(PIN_BTN) && i <= 100){
                analogWrite(D5, LinearLED[i]);
                currentBrightness = i;
                state = true;
                delay(50);
                i++;
            } 
   
        } else {      // dim down
            int i = currentBrightness;
            dimDirection = true;
            while(!digitalRead(PIN_BTN) && i >= 0){
                analogWrite(D5, LinearLED[i]);
                currentBrightness = i;
                if (currentBrightness > 0) {state = true;}
                if (currentBrightness == 0) {state = false;}
                delay(50);
                i--;
                } 
        }
        
        duringLongClick = true;
    }

    //button released after a long press
    if(!boolButtonPressed && duringLongClick && (currentMillis - lastMillis >= longClickDuration)){
        duringLongClick = false;
           
        PublishStatus();
    }
}

void setBrightness(int newBrightness) {
     
  // This function will change brightness from last known brightness to the new one

  if (newBrightness > currentBrightness ) {

    int i;
    for (i=currentBrightness; newBrightness>i; i++) {
      analogWrite(D5, LinearLED[i]);
      delay(20);
     }
     
  } else if (newBrightness < currentBrightness ) {

    int i;
    for (i=currentBrightness; newBrightness<i; i--) {
      analogWrite(D5, LinearLED[i]);
      delay(20);
   }

  } else if (newBrightness == currentBrightness ) {

     analogWrite(D5, LinearLED[newBrightness]);
  }
}

void blink() {

  //Blink on received MQTT message
  digitalWrite(BUILTIN_LED, LOW);
  delay(25);
  digitalWrite(BUILTIN_LED, HIGH);
}


void setup() {
  pinMode(PIN_LED, OUTPUT);         // Setup pin for MOSFET
  pinMode(PIN_BTN, INPUT);          // Setup pin for button
  analogWriteRange(255);            // This should set PWM range not 1023 but 100 as is %
  analogWrite(D5,0);                // Turn OFF by default
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);;
  setup_wifi();
  setup_ota();
  client.setCallback(callback);
  digitalWrite(BUILTIN_LED, HIGH);   // Turn off led as default
}


void loop() {

  if (!client.connected()) {
      reconnect();
    }
    client.loop();
    ArduinoOTA.handle();
    brightnessButton();
}
