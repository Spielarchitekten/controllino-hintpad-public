/*
   switch functions for controllino
*/

#include <Arduino.h>

// switch digital pins and relays
void turnDR ( int thePin, int newValue, int outputType, int posInArray) {
  String serString; // string that will be sent via serial
  // compose serial string
  if (outputType == 1) {
    
      serString = "[D";
      serString += posInArray;
      serString += ",";
      serString += newValue;
      serString += "]";
      
    if (posInArray < 20) {
      digitalWrite(outputD[posInArray], newValue); // switch the pin
    } else {

      if (posInArray == 20) {
        if (newValue == 1) {
          PORTD = PORTD | B00010000;
        } else {
          PORTD = PORTD & B11101111;
        }
      } else if (posInArray == 21) {
        if (newValue == 1) {
          PORTD = PORTD | B00100000;
        } else {
          PORTD = PORTD & B11011111;
        }
      } else if (posInArray == 22) {
        if (newValue == 1) {
          PORTD = PORTD | B01000000;
        } else {
          PORTD = PORTD & B10111111;
        }
      } else if (posInArray == 23) {
       
#if CMODL != 1        
        if (newValue == 1 && CMODL != 1) {
          PORTJ = PORTJ | B00010000;
        } else {
          PORTJ = PORTJ & B11101111;
        }
#endif
      }
    }

  } else if (outputType == 2) {
    serString = "[R";
    serString += posInArray;
    serString += ",";
    serString += newValue;
    serString += "]";
    digitalWrite(outputR[posInArray], newValue); // switch the pin
  } else {
    serString = "[ERROR: PIN DOES NOT EXIST]";
  }
  Serial.println(serString); // REPORT via serial
}

