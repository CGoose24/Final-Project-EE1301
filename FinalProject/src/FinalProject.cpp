#include "Particle.h"
#include "neopixel.h"
#include <string>
#include <cstring>

using namespace std;

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);

//Some commented stuff is from IoT4 for reference



                                                            ////VARIABLES:////

int motionPIN = D2; //motion sensor pin 

bool previousSensorState; 
bool motionLogged; //used to ensure only 1 log (or output message) per detection
int detectionCount;

string timeStamp; //Learned time functions and formatting from: https://docs.particle.io/reference/device-os/api/time/zone/

//LED configuration:
int PIXEL_COUNT = 3;
#define PIXEL_PIN SPI // S0 Pin
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

//color presets for LEDs
int PixelColorRed  = strip.Color(   0,  255,   0);
int PixelColorGreen  = strip.Color(   255,  0,   0);
int PixelOFF  = strip.Color(   0,  0,   0);

//LED functions
void RedLEDs();
void GreenLEDs();
void colorWipeForward(uint32_t c, uint8_t wait);
void colorWipeBackwards(uint32_t c, uint8_t wait);

//serial port detection message for testing
void detectionMessage(int detectionCount, string timeStamp);


//int setModeFromString(String inputString); //cloud function calls this
//int setTargetTempFromString(String inputString);



 
                                                            ////SETUP:////

void setup() {

  pinMode(motionPIN, INPUT);
  Serial.begin(9600);
  strip.begin();

  detectionCount = 0;
  previousSensorState = false; 
  motionLogged = false; 

  //FIXME: timezone not working  correctly
  //to make sure time is accurate:
  //waitUntil(Particle.connected);
  //Particle.syncTime();
  //Time.zone(-5); // Converts particle time zone to Central Time (from UTC)
  //waitFor(Time.isValid, 10000);  // Wait up to 10s for time to sync
  
  
  
  //Particle.variable("DetectionCount", detectionCount);
  //Particle.variable("DetectionTime", detectionTime); //Use some type of c++ function to record time
  
  //Particle.variable("cV_targetTemp", targetTemp);

  //Particle.function("cF_SetMode", setModeFromString); //declares particle function that calls setModeFromString
  //Particle.function("cF_setTargetTemp", setTargetTempFromString);



}

                                                            ////LOOP:////

void loop() {

  int motionReading = digitalRead(motionPIN);

  //IF: motion was just detected:
  if(motionReading == HIGH && previousSensorState == false) {  

    previousSensorState = true;
    motionLogged = false; 
    RedLEDs(); 

    //only want motion to be logged once
    if(motionLogged == false) {
      detectionCount++;

      //record the time of motion detection (for detection log on website)
      timeStamp = Time.format(Time.local(), "%m/%d/%Y %I:%M:%S %p").c_str(); //Formats timestamp as month/day/year, hours:mins:secs am/pm
      detectionMessage(detectionCount, timeStamp); //output motion detected message (for debugging and testing)

      motionLogged = true; //wont retrigger

    }

  //ELSE IF: motion just stopped being detected:
  } else if(motionReading == LOW && previousSensorState == true) {

    previousSensorState = false;
    GreenLEDs(); 

  }
}




                                                            ////FUNCTIONS:////

void RedLEDs() {
    colorWipeForward(strip.Color(0, 255, 0), 30); //cool animation from neopixel library
    strip.setPixelColor(0, PixelColorRed);
    strip.setPixelColor(1, PixelColorRed);
    strip.setPixelColor(2, PixelColorRed);
    strip.show();
}

void GreenLEDs() {
    colorWipeBackwards(strip.Color(255, 0, 0), 30); 
    strip.setPixelColor(0, PixelColorGreen);
    strip.setPixelColor(1, PixelColorGreen);
    strip.setPixelColor(2, PixelColorGreen);
    strip.show();
}

//FROM NEOPIXEL LIBRARY EXAMPLES
void colorWipeForward(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//slightly modified to wipe in reverse direction
void colorWipeBackwards(uint32_t c, uint8_t wait) {
  for(uint16_t i=strip.numPixels(); i > 0; i--) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void detectionMessage(int detectionCount, string timeStamp) {
      Serial.print("MOTION DETECTION ");
      Serial.print(detectionCount);
      Serial.print(" | ");
      Serial.println(timeStamp.c_str()); //serial print only works with c strings
}