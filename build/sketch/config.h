#line 1 "/home/magraina/projects/truckBeaconLightModule/config.h"
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

/************************************
 * Configuration Programm
 ************************************/
#define SERIAL_COM true							// Activate Communication to other modules via Serial 
#define BEACON_SAMPLETIME 500					// time for one run through (all 4 LEDs)
#define BEACON_TIME_VARIATION 10				// time variation between both beacon lights
//Change this value for different debuging levels
#define DEBUGLEVEL 1						////1 = Status LED | >2 = Serial Monitor

#endif