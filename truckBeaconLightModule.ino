/************************************ 
 * truckBeaconLightModule v0.0.8
 * Date: 10.05.2020 | 17:09
 * <Beacon Light Module for Truck Light and function module>
 * Copyright (C) 2020 Marina Egner <info@sheepindustries.de>
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the 
 * Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see <https://www.gnu.org/licenses/>.
 ************************************/

/************************************
 * Configuration Programm
 ************************************/
#define wireCom true                     	//Activate Communication to other modules via I2C 
#if (wireCom == true)
	#define truckAdress 1                     	//I2C Adress for Module: Truck
	#define beaconAdress 2                  	//I2C Adress for this Module: Beacon Lights Extension
	#define trailerAdress 3                  	//I2C Adress for Module: Trailer 
	#define extFunctionAdress 4               	//I2C Adress for Module: Special function for example Servos Steering
#endif
#define numberOfBeacons 2
#define beaconSampleTime 1000				//time for one run through (all 4 LEDs)
#define beaconTimeVariation beaconSampleTime/10
#define debugLevel 3						//
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
#if (debugLevel >=1)
	#define outStatusLed 13
#endif
#if (numberOfBeacons >= 1)                   	//Arduino status LED output Pin
	#define outFirstBeaconLight1 4            	//First Beacon light 1 output pin
	#define outFirstBeaconLight2 5            	//First Beacon light 2 output pin
	#define outFirstBeaconLight3 6           	//First Beacon light 3 output pin
	#define outFirstBeaconLight4 7           	//First Beacon light 4 output pin
#endif
#if (numberOfBeacons >= 2) 
	#define outSecondBeaconLight1 8           	//Second Beacon light 1 output pin
	#define outSecondBeaconLight2 9           	//Second Beacon light 2 output pin
	#define outSecondBeaconLight3 10          	//Second Beacon light 3 output pin
	#define outSecondBeaconLight4 11          	//Second Beacon light 4 output pin
#endif
#if (numberOfBeacons >= 3) 
	#define outThirdBeaconLight1 12            	//Third Beacon light 1 output pin
	#define outThirdBeaconLight2 13            	//Third Beacon light 2 output pin
	#define outThirdBeaconLight3 14           	//Third Beacon light 3 output pin
	#define outThirdBeaconLight4 15           	//Third Beacon light 4 output pin
#endif
#if (numberOfBeacons >= 4) 
	#define outFourthBeaconLight1 16           	//Fourth Beacon light 1 output pin
	#define outFourthBeaconLight2 17           	//Fourth Beacon light 2 output pin
	#define outFourthBeaconLight3 18          	//Fourth Beacon light 3 output pin
	#define outFourthBeaconLight4 19          	//Fourth Beacon light 4 output pin
#endif

//Free IOs 18, 19
/************************************
 * Definition and Initialisation 
 * Global Vars, Classes and Functions
 ************************************/
bool pulseStatus = false;
unsigned int RecivedRegisterAdress = 0;
#define maxRegisterAdress 10
unsigned int RecivedData[maxRegisterAdress] = { 0 };
unsigned int RecivedCRC = 0;
unsigned int answerToMaster = 0;
unsigned long statusPreviousMillis = 0;
#define singleBeaconSampleTime beaconSampleTime/4

#define beaconSampleTime2 beaconSampleTime+beaconTimeVariation
#define singleBeaconSampleTime2 beaconSampleTime2/4

#define beaconSampleTime3 beaconSampleTime-beaconTimeVariation
#define singleBeaconSampleTime3 beaconSampleTime3/4

#define beaconSampleTime4 beaconSampleTime2+beaconTimeVariation
#define singleBeaconSampleTime4 beaconSampleTime4/4
//Functions
bool controllerStatus(bool);
void beaconLight(const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
void beaconLightOff(const unsigned int, const unsigned int, const unsigned int, const unsigned int);
void receiveEvent(int);
bool checkCRC(unsigned int, unsigned int, unsigned int);

void setup() {
  // put your setup code here, to run once:
	#if (wireCom == true)
	Wire.begin(beaconAdress);                 	// join I2C bus (address optional for master)
	Wire.onReceive(receiveEvent); 				// register event
	Wire.onRequest(requestEvent); 				// register event
	#endif
	#if (debugLevel >=2)
	Serial.begin(9600);  // start serial for output
	#endif
	

	#if (numberOfBeacons >= 1) 
	SoftPWMBegin();                           	//Init Soft PWM Lib
	SoftPWMSet(outFirstBeaconLight1, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight1, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight2, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight2, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight3, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight3, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight4, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight4, singleBeaconSampleTime, singleBeaconSampleTime);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	#endif
	#if (numberOfBeacons >= 2) 
	SoftPWMSet(outSecondBeaconLight1, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight1, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight2, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight2, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight3, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight3, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight4, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight4, singleBeaconSampleTime2, singleBeaconSampleTime2);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	#endif
	#if (numberOfBeacons >= 3) 
	SoftPWMSet(outThirdBeaconLight1, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outThirdBeaconLight1, singleBeaconSampleTime3, singleBeaconSampleTime3);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outThirdBeaconLight2, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outThirdBeaconLight2, singleBeaconSampleTime3, singleBeaconSampleTime3);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outThirdBeaconLight3, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outThirdBeaconLight3, singleBeaconSampleTime3, singleBeaconSampleTime3);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outThirdBeaconLight4, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outThirdBeaconLight4, singleBeaconSampleTime3, singleBeaconSampleTime3);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	#endif
	#if (numberOfBeacons >= 4)   
	SoftPWMSet(outFourthBeaconLight1, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outFourthBeaconLight1, singleBeaconSampleTime4, singleBeaconSampleTime4);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFourthBeaconLight2, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outFourthBeaconLight2, singleBeaconSampleTime4, singleBeaconSampleTime4);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFourthBeaconLight3, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outFourthBeaconLight3, singleBeaconSampleTime4, singleBeaconSampleTime4);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFourthBeaconLight4, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outFourthBeaconLight4, singleBeaconSampleTime4, singleBeaconSampleTime4);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	#endif
	
}

void loop() {                             		// put your main code here, to run repeatedly:
	#if (debugLevel >=1)
	bool errorFlag = false;                 	// local var for error status
	#endif
	if(RecivedData[1]) {
		#if (numberOfBeacons >= 1) 
		beaconLight(outFirstBeaconLight1, outFirstBeaconLight2, outFirstBeaconLight3, outFirstBeaconLight4, beaconSampleTime);
		#endif
		#if (numberOfBeacons >= 2) 
		beaconLight(outSecondBeaconLight1, outSecondBeaconLight2, outSecondBeaconLight3, outSecondBeaconLight4, singleBeaconSampleTime2);
		#endif
		#if (numberOfBeacons >= 3) 
		beaconLight(outThirdBeaconLight1, outThirdBeaconLight2, outThirdBeaconLight3, outThirdBeaconLight4, singleBeaconSampleTime3);
		#endif
		#if (numberOfBeacons >= 4) 
		beaconLight(outFourthBeaconLight1, outFourthBeaconLight2, outFourthBeaconLight3, outFourthBeaconLight4, singleBeaconSampleTime4);
		#endif
	} else {
		#if (numberOfBeacons >= 1) 
		beaconLightOff(outFirstBeaconLight1, outFirstBeaconLight2, outFirstBeaconLight3, outFirstBeaconLight4);
		#endif
		#if (numberOfBeacons >= 2) 
		beaconLightOff(outSecondBeaconLight1, outSecondBeaconLight2, outSecondBeaconLight3, outSecondBeaconLight4);
		#endif
		#if (numberOfBeacons >= 3) 
		beaconLightOff(outThirdBeaconLight1, outThirdBeaconLight2, outThirdBeaconLight3, outThirdBeaconLight4);
		#endif
		#if (numberOfBeacons >= 4) 
		beaconLightOff(outFourthBeaconLight1, outFourthBeaconLight2, outFourthBeaconLight3, outFourthBeaconLight4);
		#endif
	}

	
	// Example For later Communication with other Module
	// TODO: Setup Communication
	#if (wireCom == true)
	//wireComunication();
	#endif
	#if (debugLevel >=1)
	controllerStatus(errorFlag);			//function to signal actual status with status led
	#endif
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
void beaconLightOff(const unsigned int pin1, const unsigned int pin2, const unsigned int pin3, const unsigned int pin4) {
	SoftPWMSet(pin1, 0);
	SoftPWMSet(pin3, 0);
	SoftPWMSet(pin2, 0);
	SoftPWMSet(pin4, 0);
	
}
#if (debugLevel >=1)
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
#endif

#if (wireCom == true)
void receiveEvent(int howMany) {
	unsigned int tempRecivedData = 0;
	if(howMany == 3){
		while (1 <= Wire.available()) { // loop through all 
			RecivedRegisterAdress = Wire.read(); // receive byte as a character
			tempRecivedData = Wire.read(); // receive byte as a character
			RecivedCRC = Wire.read(); // receive byte as a character
			#if (debugLevel >=3)
			Serial.println("Recived Data:");         // print the character
			Serial.println(RecivedRegisterAdress);   // print the character
			Serial.println(tempRecivedData);         // print the character
			Serial.println(RecivedCRC);         	 // print the character
			#endif
		}
		
		if((checkCRC(RecivedRegisterAdress, tempRecivedData, RecivedCRC)) && (RecivedRegisterAdress < maxRegisterAdress)) { //Prüfe CRC und größe der adresse
			RecivedData[RecivedRegisterAdress] = tempRecivedData;		//Wenn alles in Ordung, dann schreibe
			answerToMaster = 0x10; //Status Code Everything is fine
			#if (debugLevel >=3)
			Serial.println("Error Code Everything is fine");         // print the character
			#endif
		} else {
			answerToMaster = 0x11; //Error Code CRC Check failed
			#if (debugLevel >=3)
			Serial.println("Error Code CRC Check failed");         // print the character
			#endif
		}
	} else {
		//Error to much information
		while (1 <= Wire.available()) { // loop through all 
			unsigned int trashData = Wire.read(); // receive data an forget it
		}
		answerToMaster = 0x12;     //Error Code to much information
		#if (debugLevel >=3)
		Serial.println("Error Code to much information");         // print the character
		#endif
	}
	
}

bool checkCRC(unsigned int RecivedRegisterAdress, unsigned int tempRecivedData, unsigned int RecivedCRC){
	if(RecivedCRC){
		return true;
	} else {
		return false;
	}
	
}

void requestEvent() {
	#if (debugLevel >=3)
	Serial.println("Requenst from Master");         // print the character
	#endif
	Wire.write(answerToMaster);
}
#endif
