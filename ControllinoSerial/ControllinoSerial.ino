/*
  Main sketch for serial communication between
  Hintpad and Controllino MINI/MAXI/MEGA

  gregor [at] pfeffermind.de

  -> Send serial data to the Controllino in order to switch pins/relays on and off
  -> Controllino reports HIGH and LOW reads from input pins to Hintpad via serial connection

  Controllino PinOut sheets: https://controllino.biz/downloads/

  USAGE:
  send integers enclosed by < and > chars via serial port
  <pin, value>
  pin        the pin (native Arduino Mega) you want to switch
  value      0 = off, 1 = on

  - NOTES -

  I/O INFO:
  Prints i/o info to serial monitor enclosed in square brackets:
  [pin, value]

  INPUT PIN REPORT:
  Sends current states of all input pins via serial connection.
  usage: send "<999>" OR "<RESET CONTROLLINO>" via serial in order to report input pin states

  latest changes:
  - v1.1: support for additional input pins on MAXI/MEGA
  - v1.2.1: reports signals using IDs that are printed on the device
  - v1.2.2: button debounce fix
  - v1.2.3: - specified missing pin error message
            - added support for D20-D23 outputs on MEGA
  - v1.2.4: - small fix for reporting D20-D23 action back to Hintpad
  - v1.2.5: - exclude MINI from reporting D20-D23
  - v1.2.6: - report input pins on startup
            - code cleanup
  - v1.2.7: - added 'report input' function
*/

String currentVersion = "1.2.7";

#include <Controllino.h>

// ****************************** //
// set up Controllino model here:
// 1 = MINI
// 2 = MAXI
// 3 = MEGA
#define CMODL 1
// ****************************** //

#if CMODL == 1
// MINI
// outputs
const int iocount1 = 8;
const int iocount2 = 6;
const int iocount3 = 8;
int outputD[] = {CONTROLLINO_D0, CONTROLLINO_D1, CONTROLLINO_D2, CONTROLLINO_D3,
                 CONTROLLINO_D4, CONTROLLINO_D5, CONTROLLINO_D6, CONTROLLINO_D7
                };

int outputR[] = {CONTROLLINO_D0, CONTROLLINO_D1, CONTROLLINO_D2, CONTROLLINO_D3,
                 CONTROLLINO_D4, CONTROLLINO_D5
                };
// inputs
int inputA[] = {A0, A1, A2, A3, A6, A7, CONTROLLINO_IN0, CONTROLLINO_IN1
               };

int inputVal[8]; // input values
int oldInputVal[8]; // old input values for debouncing
boolean isSent[8]; // sent flags

#elif CMODL == 2
// MAXI
// outputs
const int iocount1 = 12;
const int iocount2 = 10;
const int iocount3 = 12;
int outputD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int outputR[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
// inputs
int inputA[] = { CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4,
                 CONTROLLINO_A5, CONTROLLINO_A6, CONTROLLINO_A7, CONTROLLINO_A8, CONTROLLINO_A9,
                 CONTROLLINO_IN0, CONTROLLINO_IN1
               };

int inputVal[12]; // input values
int oldInputVal[12]; // old input values for debouncing
boolean isSent[12]; // sent flags
#elif CMODL == 3
// MEGA
// outputs
const int iocount1 = 24;
const int iocount2 = 16;
const int iocount3 = 21;
int outputD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 42, 43, 44, 45, 46, 47, 48, 49, 120, 121, 122, 123};
int outputR[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
// inputs
int inputA[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4,
                CONTROLLINO_A5, CONTROLLINO_A6, CONTROLLINO_A7, CONTROLLINO_A8, CONTROLLINO_A9,
                CONTROLLINO_A10, CONTROLLINO_A11, CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14, CONTROLLINO_A15,
                CONTROLLINO_I16, CONTROLLINO_I17, CONTROLLINO_I18, CONTROLLINO_IN0, CONTROLLINO_IN1
               };

int inputVal[iocount3]; // input values
int oldInputVal[iocount3]; // old input values for debouncing
boolean isSent[iocount3]; // sent flags
#endif


// PORT MANIPULATION FOR MEGA D20-D23 *
// in order to use the non-arduino pins D20-D23 on the MEGA
// we need to manipulate the register ports directly
int MEGA_PINS_20_23 = B01110000; // DDRD
int MEGA_PINS_24 = B00010000;    // DDRJ
// * PORT MANIPULATION FOR MEGA D20-D23


// button de-bounce
int long debounceMS [iocount3]; // stores the current millis()-values for each input
int doDebounce [iocount3]; // button input debounce flags
const int debounceTime = 250; // in ms

#include "swfunc.h"
#include "listenerfunc.h"

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port
  }

  // Welcome message - DO NOT EDIT THIS!
  Serial.println  ("CONTROLLINO SERIAL READY!");
  Serial.println  ("Ver. " + currentVersion);
  Serial.println  ("Built on " __DATE__ " at " __TIME__);
  Serial.print    ("Controllino model #");  Serial.println ( CMODL );
  Serial.println  ("----------------");
  Serial.print    ("Digital Pins: ");       Serial.println ( iocount1 );
  Serial.print    ("Relay Pins: ");         Serial.println ( iocount2 );
  Serial.print    ("Input Pins: ");         Serial.println ( iocount3 );
  Serial.println  ("----------------");

  for (int i = 0; i < iocount3; i++) {
    pinMode (inputA[i], INPUT);
  }
  for (int i = 0; i < iocount2; i++) {
    pinMode (outputR[i], OUTPUT);
  }
  for (int j = 0; j < iocount1; j++) {
    pinMode (outputD[j], OUTPUT);
  }

#if CMODL != 1
  // PORT MANIPULATION FOR MEGA D20-D23
  DDRD = DDRD | MEGA_PINS_20_23;  // set up pins 47 (D20), 48(D21), 49(D22) on PORTD
  DDRJ = DDRJ | MEGA_PINS_24;     // set up pin 67 (D23) on PORTJ
#endif

  reportInputs(); // reporting input pin states on startup once
  
}

void loop() {
  listenSerial();
  listenPins();
}
