/*
 * truckBeaconLightModule v0.1.0
 * Date: 17.09.2022 | 20:13
 * <Beacon Light Module for Truck Light and function module>
 * Copyright (C) 2022 Marina Egner <info@sheepCreativeStudios.de>
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
#include "config.h"							// Config for the programm
#include "ioMapping.h"						// Config of the I/Os

/************************************
 * Include Files
 ************************************/
#include "lightFunctions.h"					// Light Functions
#include "serialCommSlave.h"				// Serial Slave Interface

/************************************
 * Definition and Initialisation 
 * Global Vars, Classes and Functions
 ************************************/
//Setup Serial and check if Board is UNO with one Serial or Leonardo/Micro with to Serials
#if defined(HAVE_HWSERIAL1)							//if serial ports 1 exist then the arduino has more than one serial port
	#ifndef SerialUSB								//if not allready defined
		#define SerialUSB SERIAL_PORT_MONITOR		//then define monitor port
	#endif
#else
	#if (DEBUGLEVEL >1)								//if serial ports are the same debuging is not possible (for example on UNO)
		#define DEBUGLEVEL 1						//do not change!!!
	#endif
#endif
#ifndef SerialHW									//if not allready defined
	#define SerialHW SERIAL_PORT_HARDWARE			//then define hardware port
#endif

// Variables
bool pulseStatus = false;
uint32_t statusPreviousMillis = 0;
#define singleBEACON_SAMPLETIME (BEACON_SAMPLETIME / 6) // Calculate time for a single of 6 LEDs
#define BEACON_SAMPLETIME2 (BEACON_SAMPLETIME + BEACON_TIME_VARIATION) // Change time for second beacon light to be different

// Functions
bool controllerStatus(bool);

void setup() {
  // put your setup code here, to run once:
	#if (SERIAL_COM == true)
	serialConfigure(&SerialHW,			// Serial interface on arduino
					19200,				// Baudrate
					SERIAL_8N1,			// e.g. SERIAL_8N1 | start bit, data bit, stop bit
					outTxEnablePin		// Pin to switch between Transmit and Receive
	);
	#endif
	#if (DEBUGLEVEL >=1)
	pinMode(outStatusLed, OUTPUT);
	#endif
	initLightOutput();					// Init SoftPWM Lib
	setupLightOutput(outFirstBeaconLight1, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10); // Create and set pin | Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	setupLightOutput(outFirstBeaconLight2, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outFirstBeaconLight3, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outFirstBeaconLight4, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outFirstBeaconLight5, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outFirstBeaconLight6, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outAuxLight, 200, 200);
	
	setupLightOutput(outSecondBeaconLight1, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10); // Create and set pin | Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
	setupLightOutput(outSecondBeaconLight2, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outSecondBeaconLight3, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outSecondBeaconLight4, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outSecondBeaconLight5, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outSecondBeaconLight6, singleBEACON_SAMPLETIME-10, singleBEACON_SAMPLETIME-10);
	setupLightOutput(outParkLight, 200, 200);		
}

void loop() {                             		// put your main code here, to run repeatedly:
	#if (DEBUGLEVEL >=1)
	bool errorFlag = false;                 	// local var for error status
	#endif

	#if (SERIAL_COM)
	serialUpdate();								// Update Data from serial communication
	uint32_t lightState = getLightState();		// Get Light State from Serial Interface
	uint16_t globalEffectState = getGlobalEffectState();	// Get Global Effect State from Serial Interface
	bool beaconLightState = (lightState & BIT_BEACON_LIGHT) != 0;
	bool parkLightState = (lightState & BIT_PARKING_LIGHT) != 0;
	bool auxLightState = (lightState & BIT_AUX1) || (globalEffectState & BIT_GLOBAL_FLASH_TO_PASS) != 0;
	#else
	bool beaconLightState = true;
	bool parkLightState = true;
	bool auxLightState = true;
	#endif
	
	if(beaconLightState) {
		// Turn lights after each other on
		beaconLight(outFirstBeaconLight1,
					outFirstBeaconLight2,
					outFirstBeaconLight3,
					outFirstBeaconLight4,
					outFirstBeaconLight5,
					outFirstBeaconLight6,
					BEACON_SAMPLETIME
		);
		beaconLight(outSecondBeaconLight1,
					outSecondBeaconLight2,
					outSecondBeaconLight3,
					outSecondBeaconLight4,
					outSecondBeaconLight5,
					outSecondBeaconLight6,
					BEACON_SAMPLETIME2
		);
	} else {
		// Trun all lights off
		beaconLightOff(outFirstBeaconLight1,
					outFirstBeaconLight2,
					outFirstBeaconLight3,
					outFirstBeaconLight4,
					outFirstBeaconLight5,
					outFirstBeaconLight6
		);
		beaconLightOff(outSecondBeaconLight1,
					outSecondBeaconLight2,
					outSecondBeaconLight3,
					outSecondBeaconLight4,
					outSecondBeaconLight5,
					outSecondBeaconLight6
		);
	}

	setBooleanLight(outAuxLight, auxLightState);
	setBooleanLight(outParkLight, parkLightState, 150);

	#if (DEBUGLEVEL >=1)
	digitalWrite(outStatusLed, controllerStatus(errorFlag));			//function to signal actual status with status led
	#endif
}

#if (DEBUGLEVEL >=1)
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
