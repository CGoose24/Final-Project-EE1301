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

int device_ON_PIN = D3; //for LED indictor 
int device_OFF_PIN = D4;
int alarm_ON_PIN = D5;
int alarm_OFF_PIN = D6;

int deviceModeIs = DEVICE_ON;
int alarmModeIs = ALARM_OFF;

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

//Time Line variables
unsigned long int T1 = millis();
unsigned long int T2 = 0;
int timeLine[12] = {0};

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
void NoLEDs();
void colorWipeForward(uint32_t c, uint8_t wait);
void colorWipeBackwards(uint32_t c, uint8_t wait);

//serial port detection message for testing
void detectionMessage(int detectionCount, String timeStamp);

//log data (to be sent to website) for each detection 
void logDetection();
void timeLineShift();

//Functions called by cloud functions
int deviceMode(String inputString); 
int alarmMode(String inputString);


                                                            ////SETUP:////

void setup() { //clean up by moving some variable initializations to the top

  pinMode(motionPIN, INPUT);

  pinMode(device_ON_PIN, OUTPUT); //TEST
  pinMode(device_OFF_PIN, OUTPUT);
  pinMode(alarm_ON_PIN, OUTPUT);
  pinMode(alarm_OFF_PIN, OUTPUT);

  Serial.begin(9600);
  strip.begin();

  detectionCount = 0;
  previousSensorState = false; 
  motionLogged = false; 
  currentMotion = false;
  timeLastBeep = 0;
  beepCount = 0;

  Time.zone(-5); // Converts particle time zone to Central Time (from UTC)

  //coud variables 
  Particle.variable("DetectionCount", detectionCount);
  Particle.variable("DetectionTime", timeStamp);
  Particle.variable("CurrentMotion", currentMotion);

  //1 hour timeline: 5 min intervals, each variabe holds the # of detections during that interval
  Particle.variable("Interval1", timeLine[0]);
  Particle.variable("Interval2", timeLine[1]);
  Particle.variable("Interval3", timeLine[2]);
  Particle.variable("Interval4", timeLine[3]);
  Particle.variable("Interval5", timeLine[4]);
  Particle.variable("Interval6", timeLine[5]);
  Particle.variable("Interval7", timeLine[6]);
  Particle.variable("Interval8", timeLine[7]);
  Particle.variable("Interval9", timeLine[8]);
  Particle.variable("Interval10", timeLine[9]);
  Particle.variable("Interval11", timeLine[10]);
  Particle.variable("Interval12", timeLine[11]);

  Particle.function("DeviceToggle", deviceMode);
  Particle.function("AlarmToggle", alarmMode);
}

                                                            ////LOOP:////

void loop() {

  
  int motionReading = digitalRead(motionPIN);

  //Device toggled ON/OFF through website
  if(deviceModeIs == DEVICE_ON) {
    digitalWrite(device_ON_PIN, HIGH);
    digitalWrite(device_OFF_PIN, LOW);

    //IF: motion was just detected:
    if(motionReading == HIGH && previousSensorState == false) {  

      currentMotion = true;
      previousSensorState = true;
      motionLogged = false; 
      RedLEDs();
  
      //only want motion to be logged once
      if(motionLogged == false) {
        logDetection();
        
        motionLogged = true; //wont retrigger
      }

    //ELSE IF: motion just stopped being detected:
    } else if(motionReading == LOW && previousSensorState == true) {

      previousSensorState = false;
      currentMotion = false;
      GreenLEDs(); //LEDS --> green

    }


    //non-blocking speaker alarm that continuously plays while motion is detected
    if(alarmModeIs == ALARM_ON) {
      digitalWrite(alarm_ON_PIN, HIGH); //alarm green LED ON
      digitalWrite(alarm_OFF_PIN, LOW); //alarm red LED OFF

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

      } else {
        digitalWrite(alarm_ON_PIN, LOW); //alarm green LED OFF
        digitalWrite(alarm_OFF_PIN, HIGH); //alarm red LED ON
      }

  } else {
    NoLEDs();
    digitalWrite(device_OFF_PIN, HIGH);
    digitalWrite(device_ON_PIN, LOW);
    digitalWrite(alarm_OFF_PIN, HIGH); //also want the alarm LED red if device is off
    digitalWrite(alarm_ON_PIN, LOW);
  }

  timeLineShift(); //shift time line array every 5 mins
}




                                                            ////FUNCTIONS:////

int deviceMode(String inputString) {
  if(inputString == "Device OFF") {
    deviceModeIs = DEVICE_OFF;
    return 0;
  } else if(inputString == "Device ON") {
    deviceModeIs = DEVICE_ON;
    GreenLEDs(); 
    return 1;
  } else {
    return -1;
  }
}

int alarmMode(String inputString){
  if(inputString == "Alarm OFF") {
    alarmModeIs = ALARM_OFF;
    return 0;
  } else if(inputString == "Alarm ON") {
    alarmModeIs = ALARM_ON;
    return 1;
  } else {
    return -1;
  }
}

void logDetection() {
  timeLine[0]++; //increments the detection count for time line
    
  //record formatted time stamp (for detection log on website)
  timeStamp = Time.format(Time.now(), "%I:%M:%S %p %m/%d/%Y "); //Formats timestamp as hours:mins:secs am/pm month/day/year
  detectionMessage(detectionCount, timeStamp); //output motion detected message (for debugging and testing)
}

void timeLineShift() { 
    //if 5 mins have passed, shift array to right -- for dynamic time line
    T1 = millis();
    if (T1 - T2 >= 300000) {
      for (int i = 11; i > 0; i--) {
        timeLine[i] = timeLine[i - 1];
      }
      timeLine[0] = 0; 
      T2 = millis();
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

void NoLEDs() {
  strip.setPixelColor(0, PixelOFF);
  strip.setPixelColor(1, PixelOFF);
  strip.setPixelColor(2, PixelOFF);
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