/************************************ 
 * truckBeaconLightModule v0.0.1
 * Date: 08.05.2020 | 21-07-10
 * by M.Egner
 * Beacon Light Module for Truck Light and function module
 ************************************/

/************************************
 * Configuration Programm
 ************************************/
#define WireCom false                     //Activate Communication to other modules via I2C 
#if (WireCom == true)
#define TruckAdress 1                     //I2C Adress for Module: Truck
#define BeaconAdress 2                    //I2C Adress for this Module: Beacon Lights Extension
#define TrailerAdress 3                   //I2C Adress for Module: Trailer 
#define ExtFunctionAdress 4               //I2C Adress for Module: Special function for example Servos Steering
#endif
                        
/************************************
 * Include Files
 ************************************/
#if (WireCom == true)
#include <Wire.h>                         //Include I2C Library
#endif

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
#define outStatusLed 13                   //Arduino status LED output Pin
#define outFirstBeaconLight1 4            //First Beacon light 1 output pin
#define outFirstBeaconLight2 5            //First Beacon light 2 output pin
#define outFirstBeaconLight3 6           //First Beacon light 3 output pin
#define outFirstBeaconLight4 7           //First Beacon light 4 output pin
#define outSecondBeaconLight1 8           //Second Beacon light 1 output pin
#define outSecondBeaconLight2 9           //Second Beacon light 2 output pin
#define outSecondBeaconLight3 10          //Second Beacon light 3 output pin
#define outSecondBeaconLight4 11          //Second Beacon light 4 output pin
#define outThirdBeaconLight1 12            //Third Beacon light 1 output pin
#define outThirdBeaconLight2 13            //Third Beacon light 2 output pin
#define outThirdBeaconLight3 14           //Third Beacon light 3 output pin
#define outThirdBeaconLight4 15           //Third Beacon light 4 output pin
#define outFourthBeaconLight1 16           //Fourth Beacon light 1 output pin
#define outFourthBeaconLight2 17           //Fourth Beacon light 2 output pin
#define outFourthBeaconLight3 18          //Fourth Beacon light 3 output pin
#define outFourthBeaconLight4 19          //Fourth Beacon light 4 output pin

//Free IOs 18, 19
/************************************
 * Definition and Initialisation 
 * Global Vars, Classes and Functions
 ************************************/
bool pulseStatus = false;
unsigned long StatusPreviousMillis = 0;
unsigned long blinkOnTime = 0;

//Functions
bool controllerStatus(bool);
int blink(unsigned int);


void setup() {
  // put your setup code here, to run once:
  #if (WireCom == true)
  Wire.begin(BeaconAdress);                 // join I2C bus (address optional for master)
  #endif
// TODO: Setup IO pins
}

void loop() {                             // put your main code here, to run repeatedly:
  bool errorFlag = false;                 // local var for error status
  
  
  // Example For later Communication with other Module
  // TODO: Setup Communication
  #if (WireCom == true)
  Wire.beginTransmission(8);             // transmit to device #8
  Wire.write("Need to Setup COM");       // sends five bytes
  Wire.endTransmission();                // stop transmitting
  #endif
  
  controllerStatus(errorFlag);
}

bool controllerStatus(bool errorFlag) {
  if(errorFlag) {
    return true;
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - StatusPreviousMillis >= 1000) { //Zeitverzoegerte Abfrage
      StatusPreviousMillis = currentMillis;
      pulseStatus = !pulseStatus;
    } else if (currentMillis < StatusPreviousMillis) {  //Reset
      StatusPreviousMillis = currentMillis; 
    }
    return pulseStatus;                 //Flash if everything is OK
  }
}

int blink(unsigned int blinkTimeMillis) {
  if((blinkOnTime == 0) || (blinkOnTime > millis())){ //Reset blinkOnTime on startup and on overflow.
    blinkOnTime = millis();
  }
  unsigned long blinkTime = millis() - blinkOnTime;
  if(blinkTime%blinkTimeMillis >= blinkTimeMillis/2){ //ON/OFF Interval at half of Time.
    return 0;
  } else {
    return 1;
  }

}
