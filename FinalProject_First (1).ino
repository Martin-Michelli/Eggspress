//Global variables (Defines, Pins, & Servos)
#include "Particle.h"
#include "neopixel.h"
#define MANUAL 0
#define CLOUD 1

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

void setup() {
pinMode (GreenButtonPIN, INPUT_PULLDOWN);
pinMode (BlueButtonPIN, INPUT_PULLDOWN);
pinMode (RedButtonPIN, INPUT_PULLDOWN);
pinMode (lightPin, OUTPUT);
pinMode (relayPin, OUTPUT);
servoOne.attach(D3);
servoTwo.attach(D1);
strip.begin ();

//CC
Particle.variable("currentMode", manualOrCloud);
Particle.function("setMode", setModeFromString);

Serial.begin(9600);
  }

void loop() {
GreenButtonNow = digitalRead(GreenButtonPIN); // Read green button state
BlueButtonNow = digitalRead(BlueButtonPIN); // Read Blue button state
RedButtonNow = digitalRead(RedButtonPIN); // Read Red button state
int PixelColorGreen = strip.Color(  0,  200,  0);
int PixelColorRed = strip.Color(  200,  0,  0);
static int counter = 0; // Toggles hot plate (Replace with bool??)

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
        }
        else {
          digitalWrite(relayPin, HIGH);
        }
      }
    else if (BlueButtonNow == LOW) {
      BlueButtonLast = LOW;
    }
  }

  //Cloud Connectivity
  if (manualOrCloud == CLOUD) {
  strip.setPixelColor(0, PixelColorRed);
  strip.show();

  /*particle functions for:
   -digitalwrite RelayPin
   -Spin both servos to high position (), delay, close servos
   */
  
  }
}