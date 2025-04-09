#include "Particle.h"
//#include "neopixel.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);


    //COMMENTED OUT STUFF is from IoT4 for reference

int motionPIN = D2; //motion sensor pin 

// This variable stores the next time to toggle the LED. 
// It will be initialized to 1 second in the future in the first iteration of the loop function
unsigned long int timeToToggle;
int sensorDelay = 1500; //change as needed
int detectionCount;

//int mode = OFF; 

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
  

  //if motion is detected and at least 1.5s have passed since last detection:
  if(motionReading == HIGH && currentTime > timeToToggle) {
  //if(lastReading != motionReading) {
      detectionCount++;

      Serial.print("MOTION DETECTED: ");
      Serial.println(detectionCount);

      timeToToggle += 5000; //test
      
 
    //}
    //lastReading = motionReading;
  }

  /*
  int CYAN = strip.Color(   100,  0,  100);
  int ORANGE = strip.Color(  100, 200, 0);
  int RED  = strip.Color(  0,   200,   0);
  int WHITE  = strip.Color(  100,   100,   100);
  */

  /*
  if( mode == COOL ){
    Serial.print("Mode: COOL, ");
    Serial.print(ADCreading);
    Serial.print(" ADC, ");
    Serial.print(tempF);
    Serial.print(" degF, ");
    Serial.print(tempC);
    Serial.println(" degC");
    delay(250);
    strip.setPixelColor(0, CYAN); //iLED glows cyan
    strip.show();
  } else if(mode == HEAT) {
    Serial.print("Mode: HEAT, ");
    Serial.print(ADCreading);
    Serial.print(" ADC, ");
    Serial.print(tempF);
    Serial.print(" degF, ");
    Serial.print(tempC);
    Serial.println(" degC");
    delay(250);
    strip.setPixelColor(0, ORANGE); //iLED glows orange
    strip.show();
  } else if(mode == REDALERT && havePublishedRedAlert == false) {
    strip.setPixelColor(0, RED); //iLED glows red for red alert
    strip.show();
    Particle.publish("RedAlertTriggered");
    havePublishedRedAlert = true; //ignores red alerts from now on
  } else {
    Serial.println("Mode: OFF");
    delay(250);
    strip.setPixelColor(0, WHITE); //iLED off
    strip.show();
  }
  */

}
/*
//handler functions like this shoudn't do much work; just updates mode variable... actually turning on/off a furnace and doing many calculations would be in loop()
int setModeFromString(String inputString){
  if(inputString == "Cool") {
    mode = COOL;
    return 0;
  } else if (inputString == "Off") {
    mode = OFF;
    return 1;
  } else if (inputString == "Heat") {
    mode = HEAT;
    return 2;
  } else if (inputString == "RedAlert") {
    mode = REDALERT;
    return 3;
  } else {
    return -1; //to indicate an error
  }
  }
*/

