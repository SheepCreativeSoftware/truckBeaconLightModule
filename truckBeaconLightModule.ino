/*
 * truckBeaconLightModule v0.0.12
 * Date: 21.06.2020 | 20:13
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
 */

/************************************
 * Configuration Programm
 ************************************/
#define serialCom false                     	//Activate Communication to other modules via Serial 
#if (serialCom == true)
	#define truckAdress 1                     	//Serial Adress for Module: Truck
	#define beaconAdress 2                  	//Serial Adress for this Module: Beacon Lights Extension
	#define trailerAdress 3                  	//Serial Adress for Module: Trailer 
	#define extFunctionAdress 4               	//Serial Adress for Module: Special function for example Servos Steering
#endif
#define beaconSampleTime 500				//time for one run through (all 4 LEDs)
#define beaconTimeVariation 10
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
	#define outStatusLed 13					//Arduino status LED output Pin
#endif

#define outFirstBeaconLight1 3            	//First Beacon light 1 output pin
#define outFirstBeaconLight2 4            	//First Beacon light 2 output pin
#define outFirstBeaconLight3 5           	//First Beacon light 3 output pin
#define outFirstBeaconLight4 6           	//First Beacon light 4 output pin
#define outFirstBeaconLight5 7            	//First Beacon light 2 output pin
#define outFirstBeaconLight6 8           	//First Beacon light 3 output pin
#define outFirstBeaconLight7 9           	//First Beacon light 4 output pin

#define outSecondBeaconLight1 12           	//Second Beacon light 1 output pin
#define outSecondBeaconLight2 11           	//Second Beacon light 2 output pin
#define outSecondBeaconLight3 10          	//Second Beacon light 3 output pin
#define outSecondBeaconLight4 A0          	//Second Beacon light 4 output pin
#define outSecondBeaconLight5 A1           	//Second Beacon light 2 output pin
#define outSecondBeaconLight6 A2          	//Second Beacon light 3 output pin
#define outSecondBeaconLight7 A3          	//Second Beacon light 4 output pin

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
bool serialIsSent = 0;
#define singleBeaconSampleTime (beaconSampleTime / 7)

#define beaconSampleTime2 (beaconSampleTime + beaconTimeVariation)
#define singleBeaconSampleTime2 (beaconSampleTime2 / 7)

//Functions
bool controllerStatus(bool);
void beaconLight(const uint8_t pin1, const uint8_t pin2, const uint8_t pin3, const uint8_t pin4, const uint8_t pin5, const uint8_t pin6, const uint8_t pin7, const uint16_t sampleTime);
void beaconLightOff(const uint8_t pin1, const uint8_t pin2, const uint8_t pin3, const uint8_t pin4, const uint8_t pin5, const uint8_t pin6, const uint8_t pin7);
//void receiveEvent(int16_t);
//bool checkCRC(uint16_t, uint16_t, uint16_t);

void setup() {
  // put your setup code here, to run once:
	#if (serialCom == true)
	SerialHW.begin(115200);  // start Serial for Communication
	#endif
	#if (debugLevel >=1)
	pinMode(outStatusLed, OUTPUT);
	#endif
	#if (debugLevel >=2)
	SerialUSB.begin(9600);  // start serial for output
	#endif

	SoftPWMBegin();                           	//Init Soft PWM Lib
	SoftPWMSet(outFirstBeaconLight1, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight1, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight2, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight2, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight3, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight3, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight4, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight4, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight5, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight5, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight6, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight6, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outFirstBeaconLight7, 0);     	//Create and set pin to 0
	SoftPWMSetFadeTime(outFirstBeaconLight7, singleBeaconSampleTime-10, singleBeaconSampleTime-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time

	SoftPWMSet(outSecondBeaconLight1, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight1, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight2, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight2, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight3, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight3, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight4, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight4, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight5, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight5, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight6, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight6, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	SoftPWMSet(outSecondBeaconLight7, 0);     //Create and set pin to 0
	SoftPWMSetFadeTime(outSecondBeaconLight7, singleBeaconSampleTime2-10, singleBeaconSampleTime2-10);       //Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time

		
}

void loop() {                             		// put your main code here, to run repeatedly:
	#if (debugLevel >=1)
	bool errorFlag = false;                 	// local var for error status
	#endif
	
	
	if((millis()%100000 >= 1000) && (serialIsSent == false)) {
			RecivedData[1] = true;
			serialIsSent = true;
		} else if((millis()%100000 < 1000) && (serialIsSent == true)) {
			serialIsSent = false;			//Stellt sicher, dass Code nur einmal je Sekunde ausgeführt wird.
			RecivedData[1] = false;
		}
	if(RecivedData[1]) {
		beaconLight(outFirstBeaconLight1, outFirstBeaconLight2, outFirstBeaconLight3, outFirstBeaconLight4, outFirstBeaconLight5, outFirstBeaconLight6, outFirstBeaconLight7, beaconSampleTime);
		beaconLight(outSecondBeaconLight1, outSecondBeaconLight2, outSecondBeaconLight3, outSecondBeaconLight4, outSecondBeaconLight5, outSecondBeaconLight6, outSecondBeaconLight7, beaconSampleTime2);
	} else {
		beaconLightOff(outFirstBeaconLight1, outFirstBeaconLight2, outFirstBeaconLight3, outFirstBeaconLight4, outFirstBeaconLight5, outFirstBeaconLight6, outFirstBeaconLight7);
		beaconLightOff(outSecondBeaconLight1, outSecondBeaconLight2, outSecondBeaconLight3, outSecondBeaconLight4, outSecondBeaconLight5, outSecondBeaconLight6, outSecondBeaconLight7);
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



void beaconLight(const uint8_t pin1, const uint8_t pin2, const uint8_t pin3, const uint8_t pin4, const uint8_t pin5, const uint8_t pin6, const uint8_t pin7, const uint16_t sampleTime) {
	uint32_t currentMillis = millis();
	uint16_t singleSampleTime = sampleTime/7;
	if(currentMillis%sampleTime >= singleSampleTime*6){ //ON/OFF Interval at half of Time.
		SoftPWMSet(pin6, 0);
		SoftPWMSet(pin7, 255);
	} else if(currentMillis%sampleTime >= singleSampleTime*5){ //ON/OFF Interval at half of Time.
		SoftPWMSet(pin5, 0);
		SoftPWMSet(pin6, 255);
	} else if(currentMillis%sampleTime >= singleSampleTime*4){ //ON/OFF Interval at half of Time.
		SoftPWMSet(pin4, 0);
		SoftPWMSet(pin5, 255);
	} else if(currentMillis%sampleTime >= singleSampleTime*3){ //ON/OFF Interval at half of Time.
		SoftPWMSet(pin3, 0);
		SoftPWMSet(pin4, 255);
	} else if (currentMillis%sampleTime >= singleSampleTime*2) {
		SoftPWMSet(pin2, 0);
		SoftPWMSet(pin3, 255);
	} else if (currentMillis%sampleTime >= singleSampleTime) {
		SoftPWMSet(pin1, 0);
		SoftPWMSet(pin2, 255);
	} else if (currentMillis%sampleTime < singleSampleTime) {
		SoftPWMSet(pin7, 0);
		SoftPWMSet(pin1, 255);
	}
	// #if (debugLevel >=2)
	// SerialUSB.println(sampleTime);
	// SerialUSB.println(singleSampleTime);
	// #endif
}
void beaconLightOff(const uint8_t pin1, const uint8_t pin2, const uint8_t pin3, const uint8_t pin4, const uint8_t pin5, const uint8_t pin6, const uint8_t pin7) {
	SoftPWMSet(pin1, 0);
	SoftPWMSet(pin3, 0);
	SoftPWMSet(pin2, 0);
	SoftPWMSet(pin4, 0);
	SoftPWMSet(pin5, 0);
	SoftPWMSet(pin6, 0);
	SoftPWMSet(pin7, 0);

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

/*#if (serialCom == true)
void receiveEvent(int16_t howMany) {
	uint16_t tempRecivedData = 0;
	if(howMany == 3){
		while (1 <= Wire.available()) { // loop through all 
			RecivedRegisterAdress = Wire.read(); // receive byte as a character
			tempRecivedData = Wire.read(); // receive byte as a character
			RecivedCRC = Wire.read(); // receive byte as a character
			#if (debugLevel >=3)
			SerialUSB.println("Recived Data:");         // print the character
			SerialUSB.println(RecivedRegisterAdress);   // print the character
			SerialUSB.println(tempRecivedData);         // print the character
			SerialUSB.println(RecivedCRC);         	 // print the character
			#endif
		}
		
		if((checkCRC(RecivedRegisterAdress, tempRecivedData, RecivedCRC)) && (RecivedRegisterAdress < maxRegisterAdress)) { //Prüfe CRC und größe der adresse
			RecivedData[RecivedRegisterAdress] = tempRecivedData;		//Wenn alles in Ordung, dann schreibe
			answerToMaster = 0x10; //Status Code Everything is fine
			#if (debugLevel >=3)
			SerialUSB.println("Error Code Everything is fine");         // print the character
			#endif
		} else {
			answerToMaster = 0x11; //Error Code CRC Check failed
			#if (debugLevel >=3)
			SerialUSB.println("Error Code CRC Check failed");         // print the character
			#endif
		}
	} else {
		//Error to much information
		while (1 <= Wire.available()) { // loop through all 
			uint16_t trashData = Wire.read(); // receive data an forget it
		}
		answerToMaster = 0x12;     //Error Code to much information
		#if (debugLevel >=3)
		SerialUSB.println("Error Code to much information");         // print the character
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
	SerialUSB.println("Requenst from Master");         // print the character
	#endif
	Wire.write(answerToMaster);
}
#endif*/
