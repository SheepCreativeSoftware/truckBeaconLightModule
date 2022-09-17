#line 1 "/home/magraina/projects/truckBeaconLightModule/lightFunctions.cpp"
/************************************ 
 * Copyright (C) 2022 Marina Egner <info@sheepindustries.de>
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

#include "lightFunctions.h"

void initLightOutput() {
	SoftPWMBegin(); 									//Init Soft PWM Lib
}

void setupLightOutput(uint8_t pin, uint16_t fadeOnTime, uint16_t fadeOffTime) {
	SoftPWMSet(pin, SOFT_PWM_LOW);						//Create and set pin to 0
	SoftPWMSetFadeTime(pin, fadeOnTime, fadeOffTime); 	//Set Fade on/off time for output
}

// Turn each LED after each other
void beaconLight(const uint8_t pin1,
				const uint8_t pin2,
				const uint8_t pin3,
				const uint8_t pin4,
				const uint8_t pin5,
				const uint8_t pin6,
				const uint8_t pin7,
				const uint16_t sampleTime
	) {
	uint32_t currentMillis = millis();
	uint16_t singleSampleTime = sampleTime/7;
	if(currentMillis%sampleTime >= singleSampleTime*6){ 			// On below 6/7 of samplingTime
		SoftPWMSet(pin6, 0);
		SoftPWMSet(pin7, 255);
	} else if(currentMillis%sampleTime >= singleSampleTime*5) { 	// On below 5/7 of samplingTime
		SoftPWMSet(pin5, 0);
		SoftPWMSet(pin6, 255);
	} else if(currentMillis%sampleTime >= singleSampleTime*4) { 	// On below 4/7 of samplingTime
		SoftPWMSet(pin4, 0);
		SoftPWMSet(pin5, 255);
	} else if(currentMillis%sampleTime >= singleSampleTime*3) { 	// On below 3/7 of samplingTime
		SoftPWMSet(pin3, 0);
		SoftPWMSet(pin4, 255);
	} else if (currentMillis%sampleTime >= singleSampleTime*2) {	// On below 2/7 of samplingTime
		SoftPWMSet(pin2, 0);
		SoftPWMSet(pin3, 255);
	} else if (currentMillis%sampleTime >= singleSampleTime) { 		// On below 1/7 of sampleTime
		SoftPWMSet(pin1, 0);
		SoftPWMSet(pin2, 255);
	} else if (currentMillis%sampleTime < singleSampleTime) {		// On under 1/7 of samplingTime
		SoftPWMSet(pin7, 0);
		SoftPWMSet(pin1, 255);
	}
}

// Turn all LEDs off
void beaconLightOff(const uint8_t pin1,
					const uint8_t pin2,
					const uint8_t pin3,
					const uint8_t pin4,
					const uint8_t pin5,
					const uint8_t pin6,
					const uint8_t pin7
	) {
	SoftPWMSet(pin1, 0);
	SoftPWMSet(pin2, 0);
	SoftPWMSet(pin3, 0);
	SoftPWMSet(pin4, 0);
	SoftPWMSet(pin5, 0);
	SoftPWMSet(pin6, 0);
	SoftPWMSet(pin7, 0);

}

void beaconTest(const uint8_t pin1, const uint8_t pin2, uint8_t value) {
	SoftPWMSet(pin1, value);
	SoftPWMSet(pin2, value);
}