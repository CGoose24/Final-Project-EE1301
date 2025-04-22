#include "Particle.h"
#include "neopixel.h"
#include <string>
#include <cstring>

using namespace std;

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);

//Some commented stuff is from IoT4 for reference

//learned how to use webhooks to send an email whenever motion is detected from: https://docs.particle.io/integrations/webhooks/

                                                            ////DECLARATIONS:////

#define DEVICE_ON 1
#define DEVICE_OFF 0
#define ALARM_ON 1
#define ALARM_OFF 0


int motionPIN = D2; //motion sensor pin 
int speakerPIN = A5; //speaker pin

int deviceMode = DEVICE_ON;
int alarmMode = ALARM_OFF

bool previousSensorState; 
bool motionLogged; //used to ensure only 1 log (or output message) per detection
bool currentMotion;
int detectionCount;

String timeStamp; //Learned time functions and formatting from: https://docs.particle.io/reference/device-os/api/time/zone/

//speaker variables
unsigned long int timeNow;
unsigned long int timeLastBeep;
bool highTone;
int beepCount;

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
void detectionMessage(int detectionCount, String timeStamp);

//Functions called by cloud functions
int deviceMode(String inputString); 
int alarmMode(String inputString);

//int setTargetTempFromString(String inputString);



 
                                                            ////SETUP:////

void setup() { //clean up by moving some variable initializations to the top

  pinMode(motionPIN, INPUT);
  Serial.begin(9600);
  strip.begin();

  detectionCount = 0;
  previousSensorState = false; 
  motionLogged = false; 
  currentMotion = false;
  timeLastBeep = 0;
  beepCount = 0;

  //FIXME: timezone not working  correctly
  //to make sure time is accurate:
  //waitUntil(Particle.connected);
  //Particle.syncTime();
  //Time.zone(-5); // Converts particle time zone to Central Time (from UTC)
  //waitFor(Time.isValid, 10000);  // Wait up to 10s for time to sync
  
  
  //if wanted, could make struct for detection count to group detections by time since
  Particle.variable("DetectionCount", detectionCount);
  Particle.variable("DetectionTime", timeStamp); //Use some type of c++ function to record time
  Particle.variable("CurrentMotion", currentMotion);

  Particle.function("DeviceToggle", deviceMode);
  Particle.function("AlarmToggle", alarmMode);
  
  //Particle.variable("cV_targetTemp", targetTemp);

  //Particle.function("cF_SetMode", setModeFromString); //declares particle function that calls setModeFromString
  //Particle.function("cF_setTargetTemp", setTargetTempFromString);



}

                                                            ////LOOP:////

void loop() {

  int motionReading = digitalRead(motionPIN);

  //IF: motion was just detected:
  if(motionReading == HIGH && previousSensorState == false) {  

    currentMotion = true;
    previousSensorState = true;
    motionLogged = false; 
    
    RedLEDs();
    

    //only want motion to be logged once
    if(motionLogged == false) {
      detectionCount++;

      //record the time of motion detection (for detection log on website)
      timeStamp = Time.format(Time.local(), "%m/%d/%Y %I:%M:%S %p"); //Formats timestamp as month/day/year, hours:mins:secs am/pm
      detectionMessage(detectionCount, timeStamp); //output motion detected message (for debugging and testing)

      motionLogged = true; //wont retrigger

    }

  //ELSE IF: motion just stopped being detected:
  } else if(motionReading == LOW && previousSensorState == true) {

    previousSensorState = false;
    currentMotion = false;
    GreenLEDs(); //LEDS --> green

  }


//non-blocking speaker alarm that continuously plays while motion is detected
  if(currentMotion) {
    timeNow = millis();
    
    if(timeNow-timeLastBeep >= 250) {
      timeLastBeep = timeNow;
      noTone(speakerPIN);

      if(beepCount == 0) {
        highTone = true;
      }

      if(highTone) {
        tone(speakerPIN, 1800, 0);
        beepCount++;
        highTone = false;
      }else if(!highTone) {
        tone(speakerPIN, 1200, 0); 
        beepCount++;
        highTone = true;
      }
    }

    } else {
      noTone(speakerPIN); //speaker stops sounding
      if(!highTone) {
       highTone = true; //to stop speaker from starting on low tone
      }
    }

}




                                                            ////FUNCTIONS:////

int deviceMode(String inputString) {
  if(inputString == "Device OFF") {
    deviceMode = DEVICE_OFF;
    return 0;
  } else if(inputString == "Device ON") {
    deviceMode = DEVICE_ON;
    return 1;
  }
}

int alarmMode(String inputString){
  if(inputString == "Alarm OFF") {
    deviceMode = ALARM_OFF;
    return 0;
  } else if(inputString == "Alarm ON") {
    deviceMode = ALARM_ON;
    return 1;
  }
}

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

void detectionMessage(int detectionCount, String timeStamp) {
      Serial.print("MOTION DETECTION ");
      Serial.print(detectionCount);
      Serial.print(" | ");
      Serial.println(timeStamp); //serial print only works with c strings
}