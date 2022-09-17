# 1 "/home/magraina/projects/truckBeaconLightModule/truckBeaconLightModule.ino"
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
# 22 "/home/magraina/projects/truckBeaconLightModule/truckBeaconLightModule.ino" 2
# 23 "/home/magraina/projects/truckBeaconLightModule/truckBeaconLightModule.ino" 2

/************************************
 * Include Files
 ************************************/
# 28 "/home/magraina/projects/truckBeaconLightModule/truckBeaconLightModule.ino" 2
# 29 "/home/magraina/projects/truckBeaconLightModule/truckBeaconLightModule.ino" 2

/************************************
 * Definition and Initialisation 
 * Global Vars, Classes and Functions
 ************************************/
//Setup Serial and check if Board is UNO with one Serial or Leonardo/Micro with to Serials
# 48 "/home/magraina/projects/truckBeaconLightModule/truckBeaconLightModule.ino"
// Variables
bool pulseStatus = false;
uint32_t statusPreviousMillis = 0;



// Functions
bool controllerStatus(bool);

void setup() {
  // put your setup code here, to run once:

 serialConfigure(&Serial1 /*then define hardware port*/, // Serial interface on arduino
     19200, // Baudrate
     0x06, // e.g. SERIAL_8N1 | start bit, data bit, stop bit
     2 // Pin to switch between Transmit and Receive
 );


 pinMode(13 /*Arduino status LED output Pin*/, 0x1);




 initLightOutput(); // Init SoftPWM Lib
 setupLightOutput(5 /*First Beacon light 1 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10); // Create and set pin | Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
 setupLightOutput(6 /*First Beacon light 2 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(3 /*First Beacon light 3 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(8 /*First Beacon light 4 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(4 /*First Beacon light 2 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(9 /*First Beacon light 3 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(7 /*First Beacon light 4 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);

 setupLightOutput(11 /*Second Beacon light 1 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10); // Create and set pin | Set fade time for pin 1000 ms fade-up time, and 1000 ms fade-down time
 setupLightOutput(A1 /*Second Beacon light 2 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(A3 /*Second Beacon light 3 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(A0 /*Second Beacon light 4 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(12 /*Second Beacon light 2 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(A2 /*Second Beacon light 3 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
 setupLightOutput(10 /*Second Beacon light 4 output pin*/, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10, (500 /* time for one run through (all 4 LEDs)*/ / 7) /* Calculate time for a single of 7 LEDs*/-10);
}

void loop() { // put your main code here, to run repeatedly:

 bool errorFlag = false; // local var for error status



 serialUpdate(); // Update Data from serial communication
 bool beaconLightState = getLightData(6); // Get Light State from Serial Interface


 if(beaconLightState) {
  // Turn lights after each other on
  beaconLight(5 /*First Beacon light 1 output pin*/,
     6 /*First Beacon light 2 output pin*/,
     3 /*First Beacon light 3 output pin*/,
     8 /*First Beacon light 4 output pin*/,
     4 /*First Beacon light 2 output pin*/,
     9 /*First Beacon light 3 output pin*/,
     7 /*First Beacon light 4 output pin*/,
     500 /* time for one run through (all 4 LEDs)*/
  );
  beaconLight(11 /*Second Beacon light 1 output pin*/,
     A1 /*Second Beacon light 2 output pin*/,
     A3 /*Second Beacon light 3 output pin*/,
     A0 /*Second Beacon light 4 output pin*/,
     12 /*Second Beacon light 2 output pin*/,
     A2 /*Second Beacon light 3 output pin*/,
     10 /*Second Beacon light 4 output pin*/,
     (500 /* time for one run through (all 4 LEDs)*/ + 10 /* time variation between both beacon lights*/) /* Change time for second beacon light to be different*/
  );
 } else {
  // Trun all lights off
  beaconLightOff(5 /*First Beacon light 1 output pin*/,
     6 /*First Beacon light 2 output pin*/,
     3 /*First Beacon light 3 output pin*/,
     8 /*First Beacon light 4 output pin*/,
     4 /*First Beacon light 2 output pin*/,
     9 /*First Beacon light 3 output pin*/,
     7 /*First Beacon light 4 output pin*/
  );
  beaconLightOff(11 /*Second Beacon light 1 output pin*/,
     A1 /*Second Beacon light 2 output pin*/,
     A3 /*Second Beacon light 3 output pin*/,
     A0 /*Second Beacon light 4 output pin*/,
     12 /*Second Beacon light 2 output pin*/,
     A2 /*Second Beacon light 3 output pin*/,
     10 /*Second Beacon light 4 output pin*/
  );
 }


 digitalWrite(13 /*Arduino status LED output Pin*/, controllerStatus(errorFlag)); //function to signal actual status with status led

}


bool controllerStatus(bool errorFlag) {
 if(errorFlag) {
  return true;
 } else {
  uint32_t currentMillis = millis();
 if (currentMillis - statusPreviousMillis >= 1000) { //Zeitverzoegerte Abfrage
  statusPreviousMillis = currentMillis;
  pulseStatus = !pulseStatus;
 } else if (currentMillis < statusPreviousMillis) { //Reset
  statusPreviousMillis = currentMillis;
 }
  return pulseStatus; //Flash if everything is OK
 }
}
