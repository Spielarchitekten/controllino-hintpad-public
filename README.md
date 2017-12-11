# controllino-hintpad
Controllino MINI/MAXI/MEGA sketch for Hintpad-I/O via serial com (USB) port

## Setup
- Install Arduino IDE https://www.arduino.cc/en/main/software 
- OR Atom with PlatformIO plugin http://docs.platformio.org/en/latest/ide/atom.html
- follow the steps to install the Controllino library and hardware definitions https://github.com/CONTROLLINO-PLC/CONTROLLINO_Library#installation-guide
- connect Controllino and set up board type and serial port in Board Manager
- download or clone this repository and open controllino-serial sketch
- set up Controllino model in sketch (1 = MINI, 2 = MAXI, 3 = MEGA) - example: #define CMODL 2
- compile and upload

## Using Controllino Input pins
As for now you can use all input pins as _digital inputs_ only, i.e. if you want to use analog sensors you will need another microcontroller to process analog signals and report a HIGH signal to a Controllino input pin when the puzzle is solved OR if you want to use the pins as outputs, you will need to edit this sketch to your liking. But keep in mind that Hintpad does not support input pins as outputs!

### Logic Voltage Levels
Logic switching levels depend on the supply voltage of the Controllino!
Use the Pin-Header of non-"pure" Controllinos in order to use the native 5V logic.

| Supply Voltage | LOW | HIGH |
|-----|---------|----------|
| 12V  | 0 - 3.6V  | 9 - 13.2V |
| 24V  | 0 - 7.2V  | 18 - 26,4V |
| 5V (pin header) | 0 - 1.5V | 3 - 5V | 

see https://controllino.biz/controllino/learning/ for more info
