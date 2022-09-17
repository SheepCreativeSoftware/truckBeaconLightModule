/************************************ 
 * Simple SerialBus Slave Interface v0.0.1
 * Date: 30.08.2022 | 21:52
 * <Truck Light and function module>
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

#ifndef _SERIAL_COMM_SLAVE_H_
#define _SERIAL_COMM_SLAVE_H_

#include "Arduino.h"							// Add base lib

// Strings for use in getLightData() for better reading
#define PARKLIGHT 0
#define BRAKELIGHT 1
#define REVERSELIGHT 2
#define RIGHTBLINK 3
#define LEFTBLINK 4
#define AUXLIGHT 5
#define BEACONLIGHT 6
#define DIMMLIGHTS 7


uint16_t serialUpdate();						// Update Data from serial communication
void serialConfigure(HardwareSerial *_SerialPort,	// Serial interface on arduino
					uint32_t baud,				// Baudrate
					uint8_t byteFormat,			// e.g. SERIAL_8N1 | start bit, data bit, stop bit
					uint8_t _TxEnablePin		// Pin to switch between Transmit and Receive
);
uint16_t calculateCRC(uint8_t bufferSize);		// Calculate CRC based on buffersize
bool getLightData(uint8_t lightOption);			// Get Light State from Serial Interface
uint16_t getServoData(uint8_t servoOption);		// Get Servo State from Serial Interface

#endif