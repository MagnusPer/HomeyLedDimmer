/*
 *  LED 12VDC PWM dimmer, remote controled by Homey home automation system and by a physical momentary switch. 
 *    
 *  Athom Homey: https://www.athom.com/en/
 *  Arduino library for communicating with Homey: https://github.com/athombv/homey-arduino-library
 *  The basis for the LED dimmer control is forked from https://github.com/Adminius/DimmerControl
 * 
 *  This example code is in the public domain.
 *  
 *  v1.0.0
 */

#include "DimmerControl.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Homey.h>

DimmerControl led;

// WiFi Setup
#define wifiSSID "xxx"        //Fill in your SSID       
#define wifiPassword "xxxx"   //Fill in your password

// OTA Setup
#define ota_port 8266
#define ota_hostname "HomeyLedDimmer"

// Homey Setup 
#define DeviceName "HomeyLedDimmer"

// PIN Setup
#define PIN_LED D5
#define PIN_BTN D3

// Global variables
bool boolButtonPressed = false;
bool duringLongClick = false;
bool dimDirection = true;     // true: dim up, false: dim down
byte debounceDuration = 60;   // ms
word longClickDuration = 400; // ms
unsigned long lastMillis = 0;
byte lastValue = 0;           // Store last value for the next short ON 
byte HomeyDimLevel = 0;       // The DIM level recived from Homey
bool LightOnOff = false;      // FALSE: Lights off, TRUE: Lights on
bool SendOnOff  = false;      
bool SendDim  = false;
bool SendDimInLoop = false;   // Fix to send value back to HomeKit app to update value

//PWM linear fade curve https://jared.geek.nz/2013/feb/linear-led-pwm
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


void setup(){
    Serial.begin(115200);
    
    // Setup WiFi
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);
    IPAddress ip(192,168,1,212);                      // Configure static IP address
    IPAddress gateway(192,168,1,1);                   // Configure static IP address
    IPAddress subnet(255,255,255,0);                  // Configure static IP address
    WiFi.config(ip, gateway, subnet);                 // Set static IP address
  
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());
    
    // Setup OTA
    ArduinoOTA.setPort(ota_port);                     // Set OTA Port - Default 8266
    ArduinoOTA.setHostname(ota_hostname);             // Set OTA Hostname
    
    // OTA Start
    ArduinoOTA.onStart([]() {                         
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
  
  
    // OTA End
    ArduinoOTA.onEnd([]() {                             
      Serial.println("\nOTA End");
    });

    // OTA Progress
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    // OTA Error
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    // OTA Startup
    ArduinoOTA.begin();

    // Setup Homey
    Homey.begin(DeviceName);                          // Unique device Name
    Homey.setClass("light");
    Homey.addCapability("onoff", setLightOnoff);      // boolean 
    Homey.addCapability("dim", setLightDim);          // number 0.00 - 1.00

    // Setup Hardware
    analogWriteRange(255);                            // Set the PWM range to 0->255 default esp8266 0->1023 

    // Setup Dimmer control 
    //led.setMinValue(0);                             // Optional: set minimum possible value (default 0)
    led.setMaxValue(100);                             // Changed to match the 0.00-1.00 (100 values) recived from Homey - set maximum possible value (default 255)
    //led.setDurationAbsolute(800);                   // Optonal: how long should it if you turn on/off? (default 500ms)
    led.setDurationRelative(4000);                    // Optonal: how long should it take if you dimming up/down (default 5000ms)
    led.setValueFunction(&setValue);                  // Mandatory: set function that should be called to control output 
   
    // Setup Pin mode
    pinMode(PIN_BTN, INPUT);
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);                       // Set LED init value to off
}

void loop(){
    ArduinoOTA.handle();                              // OTA Update
    Homey.loop();                                     // Run the Homey loop
    led.task();                                       // Task must be executed as often as posible
    taskButton();
        
    if (led.getCurrentValue() > led.getMinValue()) LightOnOff = true;
    if( led.getCurrentValue() == led.getMaxValue()) dimDirection = false;
    if (led.getCurrentValue() == led.getMinValue()){
        LightOnOff = false;
        dimDirection = true;
    }

    
    //A fix because the HomeKit(apple)app sending DIM values when light ON/OFF, needs to force return the DIM level
    if (SendDimInLoop){   
      delay(500);
      lastValue = lastValue +1;
      Homey.setCapabilityValue("dim", (lastValue*0.01)); 
      lastValue = lastValue -1;
      Homey.setCapabilityValue("dim", (lastValue*0.01));
      SendDimInLoop = false;
    }
 }

void taskButton(){
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
   
        if((currentMillis - lastMillis >= debounceDuration) &&   // Short click detected
           (currentMillis - lastMillis < longClickDuration)){
            
           if(led.getCurrentValue() == 0 && lastValue == 0){     // Max dim level if no lastValue stored
              led.taskNewValue(led.getMaxValue());               // Max DIM level                                  
              Homey.setCapabilityValue("onoff", true);           // Update Homey with ON state
              Homey.setCapabilityValue("dim", lastValue*0.01);   // Update Homey with DIM level
           }
           
           if(led.getCurrentValue() == 0 && lastValue > 0){      // Turn on with last stored ON value
              led.taskNewValue(lastValue);                       
              Homey.setCapabilityValue("onoff", true);           // Update Homey with ON state
              Homey.setCapabilityValue("dim", lastValue*0.01);   // Update Homey with DIM level
           }

           if(led.getCurrentValue() > 0){                        // Turn OFF light 
              lastValue = led.getCurrentValue();                 // Store last ON value                      
              led.taskNewValue(0);
              Homey.setCapabilityValue("onoff", false);           // Update Homey with OFF state
              Homey.setCapabilityValue("dim", 0);                 // Update Homey with DIM level
           }
                     
        }
    }

    //button is still pressed and it is a first long press detection
    if(boolButtonPressed && !duringLongClick && (currentMillis - lastMillis >= longClickDuration)){
        if(dimDirection){
            led.taskDimUp();
            dimDirection = false;
        }else{
            led.taskDimDown();
            dimDirection = true;
        }
        duringLongClick = true;
    }

    //button released after a long press
    if(!boolButtonPressed && duringLongClick && (currentMillis - lastMillis >= longClickDuration)){
        led.taskStop();
               
        if (led.getCurrentValue() > 0){                       
          Homey.setCapabilityValue("onoff", true);    // Update Homey with ON state
          Homey.setCapabilityValue("dim", (led.getCurrentValue()*0.01));   //Update Homey with Dim level
          
        }else{
          Homey.setCapabilityValue("onoff", false);   // Update Homey with OFF state
          Homey.setCapabilityValue("dim", 0);         // Update Homey with DIM level
        }
        duringLongClick = false;
    }
}

void setValue(byte value){
    analogWrite(PIN_LED, LinearLED[value]);               // Update LED
}

//Callback from Homey about Light On or Off
void setLightOnoff( void ) {                              
  if (Homey.value.toInt() == 1){                          // Light ON
      
      if (lastValue == 0){                                // When no last value stored
          led.taskNewValue(led.getMaxValue());            // Max DIM level
          Homey.setCapabilityValue("dim", (led.getMaxValue()*0.01)); 
      }
       
      if (lastValue > 0){                                 // Light ON with last stored
          led.taskNewValue(lastValue);                
          Homey.setCapabilityValue("dim", (lastValue*0.01)); 
          SendDimInLoop = true;                           // A must when using the HomeKit app to update value  
      }
  }   
   
       if (Homey.value.toInt() == 0){                     // Light off                
          lastValue = led.getCurrentValue();              // Store last ON value
          led.taskNewValue(0);                            // Turn off LEDs
          Homey.setCapabilityValue("dim", 0);             // Update Homey with DIM level
       }
}

// Callback from Homey about Light Dim level
void setLightDim( void ) {                                
    HomeyDimLevel = (Homey.value.toFloat()*100);          // Store Dim level 0-100 recived from Homey, 0.00 - 1.00 recived from Homey
   
      
    if (LightOnOff){ 
        led.taskNewValue(HomeyDimLevel);
    }
    
    if (!LightOnOff){ 
        led.taskNewValue(HomeyDimLevel);
        Homey.setCapabilityValue("dim", (HomeyDimLevel*0.01));    // Update Homey with DIM level
        Homey.setCapabilityValue("onoff", true);
     }
      
    if (HomeyDimLevel == 0){
        Homey.setCapabilityValue("onoff", false);                 // Update Homey with OFF state
        lastValue = 0;
     }
}
