/************************************ 
 * truckBeaconLightModule v0.0.9
 * Date: 08.06.2020 | 18:13
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
#define serialCom true                     	//Activate Communication to other modules via Serial 
#if (serialCom == true)
	#define truckAdress 1                     	//Serial Adress for Module: Truck
	#define beaconAdress 2                  	//Serial Adress for this Module: Beacon Lights Extension
	#define trailerAdress 3                  	//Serial Adress for Module: Trailer 
	#define extFunctionAdress 4               	//Serial Adress for Module: Special function for example Servos Steering
#endif
#define numberOfBeacons 2
#define beaconSampleTime 1000				//time for one run through (all 4 LEDs)
#define beaconTimeVariation beaconSampleTime/10
//Change this value for different debuging levels
#define debugLevel 3						////1 = Status LED | >2 = Serial Monitor
/************************************
 * Include Files
 ************************************/
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

//Free IOs 18, 19
/************************************
 * Definition and Initialisation 
 * Global Vars, Classes and Functions
 ************************************/
// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
//Setup Serial and check if Board is UNO with one Serial or Leonardo/Micro with to Serials
#if defined(HAVE_HWSERIAL1)							//if serial ports 1 exist then the arduino has more than one serial port
	#ifndef SerialUSB								//if not allready defined
		#define SerialUSB SERIAL_PORT_MONITOR		//then define monitor port
	#endif
#else
	#if (debugLevel >1)								//if serial ports are the same debuging is not possible (for example on UNO)
		#define debugLevel 1						//do not change!!!
	#endif
#endif
#ifndef SerialHW									//if not allready defined
	#define SerialHW SERIAL_PORT_HARDWARE			//then define hardware port
#endif
 
bool pulseStatus = false;
uint16_t RecivedRegisterAdress = 0;
#define maxRegisterAdress 10
uint16_t RecivedData[maxRegisterAdress] = { 0 };
uint16_t RecivedCRC = 0;
uint16_t answerToMaster = 0;
uint32_t statusPreviousMillis = 0;
#define singleBeaconSampleTime beaconSampleTime/4

#define beaconSampleTime2 beaconSampleTime+beaconTimeVariation
#define singleBeaconSampleTime2 beaconSampleTime2/4

#define beaconSampleTime3 beaconSampleTime-beaconTimeVariation
#define singleBeaconSampleTime3 beaconSampleTime3/4

#define beaconSampleTime4 beaconSampleTime2+beaconTimeVariation
#define singleBeaconSampleTime4 beaconSampleTime4/4
//Functions
bool controllerStatus(bool);
void beaconLight(const uint16_t, const uint16_t, const uint16_t, const uint16_t, const uint16_t);
void beaconLightOff(const uint16_t, const uint16_t, const uint16_t, const uint16_t);
void receiveEvent(int16_t);
bool checkCRC(uint16_t, uint16_t, uint16_t);

void setup() {
  // put your setup code here, to run once:
	#if (serialCom == true)
	SerialHW.begin(115200);  // start Serial for Communication
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
	} else {
		#if (numberOfBeacons >= 1) 
		beaconLightOff(outFirstBeaconLight1, outFirstBeaconLight2, outFirstBeaconLight3, outFirstBeaconLight4);
		#endif
		#if (numberOfBeacons >= 2) 
		beaconLightOff(outSecondBeaconLight1, outSecondBeaconLight2, outSecondBeaconLight3, outSecondBeaconLight4);
		#endif
	}

	
	// Example For later Communication with other Module
	// TODO: Setup Communication
	#if (serialCom == true)
	//serialComunication();
	#endif
	#if (debugLevel >=1)
	controllerStatus(errorFlag);			//function to signal actual status with status led
	#endif
}



void beaconLight(const uint16_t pin1, const uint16_t pin2, const uint16_t pin3, const uint16_t pin4, const uint16_t sampleTime) {
	uint32_t currentMillis = millis();
	const uint16_t singleSampleTime = sampleTime/4;
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
void beaconLightOff(const uint16_t pin1, const uint16_t pin2, const uint16_t pin3, const uint16_t pin4) {
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
		uint32_t currentMillis = millis();
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

#if (serialCom == true)
void receiveEvent(int16_t howMany) {
	uint16_t tempRecivedData = 0;
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
			uint16_t trashData = Wire.read(); // receive data an forget it
		}
		answerToMaster = 0x12;     //Error Code to much information
		#if (debugLevel >=3)
		Serial.println("Error Code to much information");         // print the character
		#endif
	}
	
}

bool checkCRC(uint16_t RecivedRegisterAdress, uint16_t tempRecivedData, uint16_t RecivedCRC){
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
