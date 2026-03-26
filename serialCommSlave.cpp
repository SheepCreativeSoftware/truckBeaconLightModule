/************************************ 
 * Simple SerialBus Master Interface v0.0.1
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

#include "serialCommSlave.h"
#include "HardwareSerial.h"

#define BUFFER_SIZE 64				// Max Serial Buffer of Arduino
#define MIN_BUFFER_SIZE 4			// Min Buffersize to evaluate data
#define BUFFER_EMPTY 0
#define FUNC_LIGHT_DATA 1			// Function if receiving Light data
#define FUNC_SERVO 2				// Function if receiving Servo data
#define MAX_TIME_SIGNAL 1000		// Maximum Time Received Data is valid
#define WAIT_FOR_NEXT_BYTE 1000		// Time in microseconds to wait for next byte while receiving data

// For CRC calculation
#define BIT_COUNT 8					// Count of bits in a byte
#define POLYNOMIAL 0xA001			// Polynomial for CRC Calculation

uint8_t frame[BUFFER_SIZE];			// Frame Buffer for Serial Data
uint8_t TxEnablePin;				// Pin to switch between Transmit and Receive
uint16_t errorCount;				// Count each invalid signal
HardwareSerial* SerialPort;			// Serial Port for communication

uint32_t lightStateFromSerial;		// Holds Light data in Binary
uint16_t globalEffectStateFromSerial;	// Holds Global Effect data in Binary
uint16_t servoMicrosFromSerial[6];	// Holds Servo Microseconds
uint32_t lastValidPackage = 0;		// Time since last valid package over serial

void serialConfigure(HardwareSerial *_SerialPort,	// Serial interface on arduino
					uint32_t baud,					// Baudrate
					uint8_t byteFormat,				// e.g. SERIAL_8N1 | start bit, data bit, stop bit
					uint8_t _TxEnablePin			// Pin to switch between Transmit and Receive
) {
	SerialPort = _SerialPort;						// Store on a global var
	(*SerialPort).begin(baud, byteFormat);			// Init communication port
	TxEnablePin = _TxEnablePin;						// Store on a global var for other functions
	pinMode(TxEnablePin, OUTPUT);
	digitalWrite(TxEnablePin, LOW);					// Set to low at start to receive data
	errorCount = 0; 								// Initialize errorCount
} 

uint16_t serialUpdate() {

	if ((*SerialPort).available()) {
		uint8_t buffer = 0;
		bool overflow = 0;

		while ((*SerialPort).available()) {
			// If more bytes is received than the BUFFER_SIZE the overflow flag will be set and the 
			// serial buffer will be red untill all the data is cleared from the receive buffer.
			if (overflow) {
				(*SerialPort).read();
			} else {
				if (buffer == BUFFER_SIZE) overflow = true;
				frame[buffer] = (*SerialPort).read();
				buffer++;
			}
			delayMicroseconds(WAIT_FOR_NEXT_BYTE); // inter character time out?
		}
		
		// If an overflow occurred increment the errorCount
		if (overflow) return errorCount++;

		if (buffer >= MIN_BUFFER_SIZE) {
			uint16_t crc16 = ((frame[buffer - 2] << 8) | frame[buffer - 1]);	// combine the crc Low & High bytes

			// if the calculated crc matches the recieved crc continue
			if (calculateCRC(buffer - 2) == crc16) {
				uint8_t function = frame[0];

				// Function 1 is Light information data which has only one byte
				if(function == FUNC_LIGHT_DATA) { 
					// 1 byte function + 4 byte lightState + 2 byte effectState +
					// 9 byte ServoData (12bit) + 2 bytes CRC => 18 bytes total
					lightStateFromSerial = ((uint32_t)frame[1])        |
											((uint32_t)frame[2] << 8)  |
											((uint32_t)frame[3] << 16) |
											((uint32_t)frame[4] << 24);
					globalEffectStateFromSerial = ((uint16_t)frame[5]) | 
													((uint16_t)frame[6] << 8); // combine the effectState bytes
					servoMicrosFromSerial[0] = ((uint16_t)frame[7] << 4) | (frame[8] >> 4);                // First 8 bits of servo 1 + top 4 bits
					servoMicrosFromSerial[1] = ((uint16_t)(frame[8] & 0x0F) << 8) | frame[9];              // Last 4 bits of servo 1's byte + last 8 bits of servo 2
					servoMicrosFromSerial[2] = ((uint16_t)frame[10] << 4) | (frame[11] >> 4);              // First 8 bits of servo 3 + top 4 bits
					servoMicrosFromSerial[3] = ((uint16_t)(frame[11] & 0x0F) << 8) | frame[12];            // Last 4 bits of servo 3's byte + last 8 bits of servo 4
					servoMicrosFromSerial[4] = ((uint16_t)frame[13] << 4) | (frame[14] >> 4);              // First 8 bits of servo 5 + top 4 bits
					servoMicrosFromSerial[5] = ((uint16_t)(frame[14] & 0x0F) << 8) | frame[15];            // Last 4 bits of servo 5's byte + last 8 bits of servo 6
				}
				lastValidPackage = millis();

			} else {
				// CRC Check failed
				errorCount++;
			}

		} else if (buffer > BUFFER_EMPTY && buffer < MIN_BUFFER_SIZE) {
			errorCount++; // corrupted packet
		}
	}
	
	// Reset everything if no valid package was there
	if((millis() - lastValidPackage) >= (uint16_t)MAX_TIME_SIGNAL) {
		lightStateFromSerial = BIT_HAZARD_LIGHT;
		globalEffectStateFromSerial = 0;
		servoMicrosFromSerial[0] = 1024;
		servoMicrosFromSerial[1] = 1024;
		servoMicrosFromSerial[2] = 1024;
		servoMicrosFromSerial[3] = 1024;
		servoMicrosFromSerial[4] = 1024;
		servoMicrosFromSerial[5] = 1024;
	}
	return errorCount;
}

uint16_t calculateCRC(uint8_t bufferSize) {
	uint16_t crc16 = 0xFFFF;														// Load a 16–bit register with FFFF hex (all 1’s)
	for (uint8_t bufferPosition = 0; bufferPosition < bufferSize; bufferPosition++) {
		crc16 = crc16 ^ frame[bufferPosition];										// XOR byte into least significant byte of crc
		for (uint8_t bitPosition = 1; bitPosition <= BIT_COUNT; bitPosition++) {	// Loop over each bit
			if(crc16 & 0x0001) {													// If the LSB is set
				crc16 >>= 1;														// Shift right and XOR with polynomial 
				crc16 ^= POLYNOMIAL;
			} else {																// Shift only to the right if LSB is not set
				crc16 >>= 1;
			}
		}
	}
	return crc16; 																	// The final content of the CRC register is the CRC value
}

// Function 1 is Light information data which has only one byte
/* 	0 -> parking light,
	1 -> brake light,
	2 -> reversing lights,
	3 -> right blinker,
	4 -> left blinker,
	5 -> auxiliary light,
	6 -> beacon light
	7 -> dimm light
*/

uint32_t getLightState() {
	return lightStateFromSerial;
}

uint16_t getGlobalEffectState() {
	return globalEffectStateFromSerial;
}