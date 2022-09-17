#line 1 "/home/magraina/projects/truckBeaconLightModule/lightFunctions.h"
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

#ifndef _LIGHT_FUNCTIONS_H_
#define _LIGHT_FUNCTIONS_H_

#include "Arduino.h"
#include <SoftPWM.h>

#define SOFT_PWM_HIGH 255
#define SOFT_PWM_LOW 0

//Functions
void initLightOutput();			// Init SoftPWM Lib
void setupLightOutput(uint8_t pin, uint16_t fadeOnTime, uint16_t fadeOffTime); // Create and set pin | Set fade up and down time for pin
void beaconLight(const uint8_t pin1,
				const uint8_t pin2,
				const uint8_t pin3,
				const uint8_t pin4,
				const uint8_t pin5,
				const uint8_t pin6,
				const uint8_t pin7,
				const uint16_t sampleTime
);
void beaconLightOff(const uint8_t pin1,
					const uint8_t pin2,
					const uint8_t pin3,
					const uint8_t pin4,
					const uint8_t pin5,
					const uint8_t pin6,
					const uint8_t pin7
);

#endif