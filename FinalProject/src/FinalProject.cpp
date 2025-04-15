#include "Particle.h"
#include "neopixel.h"
#include <ctime>
#include <string>
#include <cstring>

using namespace std;

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);


 //Most commented stuff is from IoT4 for reference

int motionPIN = D2; //motion sensor pin 

// This variable stores the next time to toggle the LED. 
unsigned long int timeToToggle;
int sensorDelay = 1500; //change as needed
int detectionCount;

void colorWipe(uint32_t c, uint8_t wait);

/*
time_t tempTime1;
tm* tempTime2;
string detectionTime;
*/

string formattedTime;

bool detection;

//iLED configuration:
int PIXEL_COUNT = 3;
#define PIXEL_PIN SPI // S0 Pin
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);


//int setModeFromString(String inputString); //cloud function calls this
//int setTargetTempFromString(String inputString);


void setup() {

  pinMode(motionPIN, INPUT);
  Serial.begin(9600);
  strip.begin();
  

  timeToToggle = millis() + sensorDelay; // Initialize the timeToToggleLED variable

  detectionCount = 0;
  detection = false;

  //to make sure time is accurate:
  waitUntil(Particle.connected);
  Particle.syncTime();
  Time.zone(); // Converts particle time zone to Central Time (from UTC)
  waitFor(Time.isValid, 10000);  // Wait up to 10s for time to sync
  
  
  /*
  Particle.variable("DetectionCount", detectionCount);
  Particle.variable("DetectionTime", detectionTime); //Use some type of c++ function to record time
  */
  //Particle.variable("cV_targetTemp", targetTemp);

  //Particle.function("cF_SetMode", setModeFromString); //declares particle function that calls setModeFromString
  //Particle.function("cF_setTargetTemp", setTargetTempFromString);



}

void loop() {
  unsigned long int currentTime = millis();  // get the current time
  int motionReading = digitalRead(motionPIN);


  int PixelColorRed  = strip.Color(   0,  255,   0);
  int PixelColorGreen  = strip.Color(   255,  0,   0);
  int PixelOFF  = strip.Color(   0,  0,   0);

 

  //int lastReading = LOW;
   
   
  if(motionReading == HIGH) {  //Add OFF mode configurable through website
    detection = true;

    //records the time of motion detection (for website)
    formattedTime = Time.format(Time.local(), "%m/%d/%Y %I:%M:%S %p").c_str(); //Formats timestamp as month/day/year, hours:mins:secs am/pm

    colorWipe(strip.Color(0, 255, 0), 50); //cool animation from neopixel library
    strip.setPixelColor(0, PixelColorRed);
    strip.setPixelColor(1, PixelColorRed);
    strip.setPixelColor(2, PixelColorRed);
    strip.show();

  } else { 
    detection = false;
    colorWipe(strip.Color(255, 0, 0), 50); //cool animation
    strip.setPixelColor(0, PixelColorGreen);
    strip.setPixelColor(1, PixelColorGreen);
    strip.setPixelColor(2, PixelColorGreen);
    strip.show();

  }

  
  //if motion is detected and at least 1.5s have passed since last detection:
  if(detection == true ) { //&& currentTime >= timeToToggle
      detectionCount++;

      Serial.print("MOTION DETECTION ");
      Serial.print(detectionCount);
      Serial.print(" | ");
      Serial.println(formattedTime.c_str()); //serial print only works with c strings
      delay(2500);

    
      //timeToToggle += sensorDelay; //test

      detection = false;
      
  }
}

//FROM NEOPIXEL LIBRARY EXAMPLES
// Fill the dots one after the other with a color, wait (ms) after each one
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}