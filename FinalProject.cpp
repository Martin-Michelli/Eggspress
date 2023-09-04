
#line 1 "c:/Users/marty/VSCodeProject/FinalProject_First/src/FinalProject_First.ino"
//Global variables (Defines, Pins, & Servos)
#include "Particle.h"
#include "neopixel.h"
int setModeFromString(String inputString);
int cloudConnectedCook(String inputStringTwo);
void setup();
void loop();
#line 4 "c:/Users/marty/VSCodeProject/FinalProject_First/src/FinalProject_First.ino"
#define MANUAL 0
#define CLOUD 1
#define WAITING 2
#define COOKING 3

int lightPin = D7;
int GreenButtonPIN = D2;
int BlueButtonPIN = D0;
int RedButtonPIN = D4;
int relayPin = D6;
int pixelPin = D5;
int pixelType = WS2811;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, pixelPin, pixelType);
Servo servoOne;
Servo servoTwo;

//Initializing button states
bool GreenButtonNow = FALSE;
bool GreenButtonLast = FALSE;
bool BlueButtonNow = FALSE;
bool BlueButtonLast = FALSE;
bool RedButtonNow = FALSE;
bool RedButtonLast = FALSE;

int onOrOff = 1;

//Cloud Connectivity
int manualOrCloud = MANUAL;
int waitingOrCooking = WAITING;
unsigned long int cloudTimer;

//Handler Function
int setModeFromString(String inputString) {
  if (inputString == "MANUAL") {
        manualOrCloud = MANUAL;
        return 0;
  }
  else if (inputString == "CLOUD") {
        manualOrCloud = CLOUD;
        return 1;
  }
  else {
    return -1; //Sentinel
  }
}

//Another handler Function
int cloudConnectedCook(String inputStringTwo) {
if (inputStringTwo == "WAITING") {
        waitingOrCooking = WAITING;
        return 2;
  }
  else if (inputStringTwo == "COOKING") {
        waitingOrCooking = COOKING;
        return 3;
  }
  else {
    return -1; //Sentinel
  }
}

void setup() {
pinMode (GreenButtonPIN, INPUT_PULLDOWN);
pinMode (BlueButtonPIN, INPUT_PULLDOWN);
pinMode (RedButtonPIN, INPUT_PULLDOWN);
pinMode (lightPin, OUTPUT);
pinMode (relayPin, OUTPUT);
servoOne.attach(D3);
servoTwo.attach(D1);
strip.begin ();
cloudTimer = millis() + 100;

//CC
Particle.variable("currentMode", manualOrCloud);
Particle.function("setMode", setModeFromString);
Particle.variable("CloudState", waitingOrCooking);
Particle.function("cookFunc", cloudConnectedCook);

Serial.begin(9600);
  }

void loop() {
GreenButtonNow = digitalRead(GreenButtonPIN); // Read green button state
BlueButtonNow = digitalRead(BlueButtonPIN); // Read Blue button state
RedButtonNow = digitalRead(RedButtonPIN); // Read Red button state
int PixelColorGreen = strip.Color(  0,  200,  0);
int PixelColorRed = strip.Color(  200,  0,  0);
int PixelColorPurple = strip.Color( 160, 32, 240 );
int PixelColorYellow = strip.Color( 100, 99, 22 );
static int counter = 0; // Toggles hot plate
static int cloudCounter = 0; //Used in the cloud cook sequence
unsigned long int currentTime = millis();

//Manual mode (Button inputs):
if (manualOrCloud == MANUAL)  {
  strip.setPixelColor(0, PixelColorGreen);
  strip.show();
  
  //Green Button (Position 1)
    if (GreenButtonNow == HIGH && GreenButtonLast == LOW) {
      GreenButtonLast = HIGH;
      servoOne.write(0);
      servoTwo.write(180);
    }
    else if (GreenButtonNow == LOW) {
      GreenButtonLast = LOW;
      digitalWrite(lightPin, LOW);
    }

  //Red Button (Position 2)
    if (RedButtonNow == HIGH && RedButtonLast == LOW) {
      servoOne.write(180);
      servoTwo.write(0);
    }
    else if (RedButtonNow == LOW) {
      RedButtonLast = LOW;
      digitalWrite(lightPin, LOW);
    }

  //Blue Button (Toggles Hot Plate)
    if (BlueButtonNow == HIGH && BlueButtonLast == LOW) {
      BlueButtonLast = HIGH;
      counter = counter + 1;
        if (counter % 2 == 0) {
          digitalWrite(relayPin, LOW);
          digitalWrite(lightPin, HIGH);
        }
        else {
          digitalWrite(relayPin, HIGH);
          digitalWrite(lightPin, LOW);
        }
      }
    else if (BlueButtonNow == LOW) {
      BlueButtonLast = LOW;
    }
  }

  //Cloud Connectivity
  if (manualOrCloud == CLOUD) {
  //changes the button breadboard light from green to red, indicating currently in CLOUD mode
  strip.setPixelColor(0, PixelColorRed);
  strip.show();
  delay (10); //So that other light signals will appear
  digitalWrite(relayPin, HIGH);

    if (waitingOrCooking == WAITING) {
      cloudCounter = 0;   //Do nothing
    }
    if (waitingOrCooking == COOKING) {
      if (cloudCounter < 1) {
        strip.setPixelColor(0, PixelColorYellow); //Yellow = "Click to cook" has been pressed
        strip.show();
 
        servoOne.write(180);
        servoTwo.write(0);
        delay(2000);
        servoOne.write(0);
        servoTwo.write(180);

        cloudTimer = currentTime + 1000; // 180000ms = 2 Minutes
      }
      cloudCounter = 3;
      if(currentTime > cloudTimer){  
        waitingOrCooking = WAITING;
        
        strip.setPixelColor(0, PixelColorPurple); //Purple = Timer finished
        strip.show();
        delay (500);
        Particle.publish("Done", "Egg has been cooked");
      }
    }
  }
}
