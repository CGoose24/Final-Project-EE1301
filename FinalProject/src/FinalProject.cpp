#include "Particle.h"
#include "neopixel.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);


 //Most commented stuff is from IoT4 for reference

int motionPIN = D2; //motion sensor pin 

// This variable stores the next time to toggle the LED. 
unsigned long int timeToToggle;
int sensorDelay = 1500; //change as needed
int detectionCount;
bool detection;


//iLED config:
int PIXEL_COUNT = 1;
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
  


  Particle.variable("DetectionCount", detectionCount);
  Particle.variable("DetectionTime", detectionTime); //Use some type of c++ function to record time

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

    strip.setPixelColor(0, PixelColorRed);
    strip.show();

  } else {
    detection = false;

    strip.setPixelColor(0, PixelColorGreen);
    strip.show();

  }

  
  //if motion is detected and at least 1.5s have passed since last detection:
  if(detection == true ) { //&& currentTime >= timeToToggle
      detectionCount++;

      Serial.print("MOTION DETECTED: ");
      Serial.println(detectionCount);
      delay(2500);

    
      //timeToToggle += sensorDelay; //test

      detection = false;
      
  }
}

