/************************************ 
 * truckBeaconLightModule v0.0.3
 * Date: 09.05.2020 | 13:03:32
 * by M.Egner
 * Beacon Light Module for Truck Light and function module
 ************************************/

/************************************
 * Configuration Programm
 ************************************/
#define wireCom false                     	//Activate Communication to other modules via I2C 
#if (wireCom == true)
#define truckAdress 1                     	//I2C Adress for Module: Truck
#define beaconAdress 2                  	//I2C Adress for this Module: Beacon Lights Extension
#define trailerAdress 3                  	//I2C Adress for Module: Trailer 
#define extFunctionAdress 4               	//I2C Adress for Module: Special function for example Servos Steering
#endif
#define debugLevel 0
#define beaconSampleTime 500				//time for one run through (all 4 LEDs)
#define beaconTimeVariation beaconSampleTime/10
/************************************
 * Include Files
 ************************************/
#if (wireCom == true)
#include <Wire.h>                         	//Include I2C Library
#endif
#include <SoftPWM.h>						//https://github.com/bhagman/SoftPWM

/************************************
 * Definition IO Pins
 ************************************/
// TODO: setup correct pins
//Pinout Arduino Micro:
//I2C 2+3 | PWM 3, 5, 6, 9, 10, 11, 13 | LED 13
//Servo Lib deactivates PWM functionality on pins 9 and 10

//Inputs
// Pin 2+3 reserved for I2C!


//Outputs
#define outStatusLed 13                   	//Arduino status LED output Pin
#define outFirstBeaconLight1 4            	//First Beacon light 1 output pin
#define outFirstBeaconLight2 5            	//First Beacon light 2 output pin
#define outFirstBeaconLight3 6           	//First Beacon light 3 output pin
#define outFirstBeaconLight4 7           	//First Beacon light 4 output pin
#define outSecondBeaconLight1 8           	//Second Beacon light 1 output pin
#define outSecondBeaconLight2 9           	//Second Beacon light 2 output pin
#define outSecondBeaconLight3 10          	//Second Beacon light 3 output pin
#define outSecondBeaconLight4 11          	//Second Beacon light 4 output pin
#define outThirdBeaconLight1 12            	//Third Beacon light 1 output pin
#define outThirdBeaconLight2 13            	//Third Beacon light 2 output pin
#define outThirdBeaconLight3 14           	//Third Beacon light 3 output pin
#define outThirdBeaconLight4 15           	//Third Beacon light 4 output pin
#define outFourthBeaconLight1 16           	//Fourth Beacon light 1 output pin
#define outFourthBeaconLight2 17           	//Fourth Beacon light 2 output pin
#define outFourthBeaconLight3 18          	//Fourth Beacon light 3 output pin
#define outFourthBeaconLight4 19          	//Fourth Beacon light 4 output pin

//Free IOs 18, 19
/************************************
 * Definition and Initialisation 
 * Global Vars, Classes and Functions
 ************************************/
bool pulseStatus = false;
unsigned long statusPreviousMillis = 0;
#define singleBeaconSampleTime beaconSampleTime/4
#define beaconSampleTime2 beaconSampleTime+beaconTimeVariation
#define singleBeaconSampleTime2 beaconSampleTime2/4
//Functions
bool controllerStatus(bool);
void beaconLight(const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int);

void setup() {
  // put your setup code here, to run once:
  #if (wireCom == true)
  Wire.begin(BeaconAdress);                 // join I2C bus (address optional for master)
  #endif

  SoftPWMBegin();                           //Init Soft PWM Lib
  SoftPWMSet(outFirstBeaconLight1, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outFirstBeaconLight1, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outFirstBeaconLight2, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outFirstBeaconLight2, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outFirstBeaconLight3, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outFirstBeaconLight3, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outFirstBeaconLight4, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outFirstBeaconLight4, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outSecondBeaconLight1, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outSecondBeaconLight1, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outSecondBeaconLight2, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outSecondBeaconLight2, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outSecondBeaconLight3, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outSecondBeaconLight3, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
  SoftPWMSet(outSecondBeaconLight4, 0);     //Create and set pin to 0
  SoftPWMSetFadeTime(outSecondBeaconLight4, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
// TODO: Setup IO pins
}

void loop() {                             	// put your main code here, to run repeatedly:
  bool errorFlag = false;                 	// local var for error status
  beaconLight(outFirstBeaconLight1, outFirstBeaconLight2, outFirstBeaconLight3, outFirstBeaconLight4, beaconSampleTime);
  beaconLight(outSecondBeaconLight1, outSecondBeaconLight2, outSecondBeaconLight3, outSecondBeaconLight4, singleBeaconSampleTime2);
  
  // Example For later Communication with other Module
  // TODO: Setup Communication
  #if (wireCom == true)
  Wire.beginTransmission(8);             	// transmit to device #8
  Wire.write("Need to Setup COM");       	// sends five bytes
  Wire.endTransmission();                	// stop transmitting
  #endif
  
  controllerStatus(errorFlag);
}

void beaconLight(const unsigned int pin1, const unsigned int pin2, const unsigned int pin3, const unsigned int pin4, const unsigned int sampleTime) {
  unsigned long currentMillis = millis();
  const unsigned int singleSampleTime = sampleTime/4;
  if(currentMillis%sampleTime >= singleSampleTime*3){ //ON/OFF Interval at half of Time.
    SoftPWMSet(pin3, 0);
    SoftPWMSet(pin4, 255);
  } else if (currentMillis%sampleTime >= singleSampleTime*2) {
    SoftPWMSet(pin2, 0);
    SoftPWMSet(pin3, 255);
  } else if (currentMillis%sampleTime >= singleSampleTime) {
    SoftPWMSet(pin1, 0);
    SoftPWMSet(pin2, 255);
  } else if (currentMillis%sampleTime < singleSampleTime) {
    SoftPWMSet(pin4, 0);
    SoftPWMSet(pin1, 255);
  }
}

bool controllerStatus(bool errorFlag) {
  if(errorFlag) {
    return true;
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - statusPreviousMillis >= 1000) { //Zeitverzoegerte Abfrage
      statusPreviousMillis = currentMillis;
      pulseStatus = !pulseStatus;
    } else if (currentMillis < statusPreviousMillis) {  //Reset
      statusPreviousMillis = currentMillis; 
    }
    return pulseStatus;                 	//Flash if everything is OK
  }
}
