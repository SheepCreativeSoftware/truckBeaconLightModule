#line 1 "/home/magraina/projects/truckBeaconLightModule/ioMapping.h"
/************************************ 
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
 ************************************/

#ifndef _IO_MAPPING_H_
#define _IO_MAPPING_H_

// TODO: setup correct pins
//Pinout Arduino Micro:
//I2C 2+3 | PWM 3, 5, 6, 9, 10, 11, 13 | LED 13
//Servo Lib deactivates PWM functionality on pins 9 and 10

//Inputs
// Pin 2+3 reserved for I2C!
#define outTxEnablePin 2

//Outputs
#define outStatusLed 13						//Arduino status LED output Pin

#define outFirstBeaconLight1 3				//First Beacon light 1 output pin
#define outFirstBeaconLight2 4				//First Beacon light 2 output pin
#define outFirstBeaconLight3 5				//First Beacon light 3 output pin
#define outFirstBeaconLight4 6				//First Beacon light 4 output pin
#define outFirstBeaconLight5 7				//First Beacon light 2 output pin
#define outFirstBeaconLight6 8				//First Beacon light 3 output pin
#define outFirstBeaconLight7 9				//First Beacon light 4 output pin

#define outSecondBeaconLight1 12			//Second Beacon light 1 output pin
#define outSecondBeaconLight2 11			//Second Beacon light 2 output pin
#define outSecondBeaconLight3 10			//Second Beacon light 3 output pin
#define outSecondBeaconLight4 A0			//Second Beacon light 4 output pin
#define outSecondBeaconLight5 A1			//Second Beacon light 2 output pin
#define outSecondBeaconLight6 A2			//Second Beacon light 3 output pin
#define outSecondBeaconLight7 A3			//Second Beacon light 4 output pin

//Free IOs 18, 19

#endif