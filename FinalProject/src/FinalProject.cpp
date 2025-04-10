#include "Particle.h"
//#include "neopixel.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);


 //Most commented stuff is from IoT4 for reference

int motionPIN = D2; //motion sensor pin 

// This variable stores the next time to toggle the LED. 
unsigned long int timeToToggle;
int sensorDelay = 1500; //change as needed
int detectionCount;


/*
//iLED config:
int PIXEL_COUNT = 1;
#define PIXEL_PIN SPI // S0 Pin
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
*/

//int setModeFromString(String inputString); //cloud function calls this
//int setTargetTempFromString(String inputString);


void setup() {

  pinMode(motionPIN, INPUT);

  Serial.begin(9600);


  timeToToggle = millis() + sensorDelay; // Initialize the timeToToggleLED variable

  detectionCount = 0;


   

  //Particle.variable("tempC", tempC);
  //Particle.variable("cV_targetTemp", targetTemp);

  //Particle.function("cF_SetMode", setModeFromString); //declares particle function that calls setModeFromString
  //Particle.function("cF_setTargetTemp", setTargetTempFromString);

  //strip.begin(); //setup iLED

}

void loop() {
  unsigned long int currentTime = millis();  // get the current time

  int motionReading = digitalRead(motionPIN);

  //int lastReading = LOW;
  


  //its detecting way too many times, despite the timer which is meant to fix it
  
  //if motion is detected and at least 1.5s have passed since last detection:
  if(motionReading == HIGH && currentTime > timeToToggle) {
      detectionCount++;

      Serial.print("MOTION DETECTED: ");
      Serial.println(detectionCount);

      timeToToggle += 5000; //test
      

  }
}


