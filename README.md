# swdFirmwareExtractor running on Teensy 3.1/3.2

This is a port of swdFirmwareExtractor to the Teensy3.1/3.2 board.

* swdFirmwareExtractor is a tool used to extract protected firmware from STM32F0 microcontrollers.
  * Originally published here: https://www.aisec.fraunhofer.de/en/FirmwareProtection.html
  * Other forks:
    * https://github.com/bikemike/swdFirmwareExtractor
    * https://github.com/romanoLT/swdFirmwareExtractor
    * https://github.com/sergachev/swdFirmwareExtractor

* Teensy 3.1/3.2 is a small development board, published by PJRC: https://www.pjrc.com/store/teensy32.html
  * The following pins are used:
    #define PIN_SWDIO 19
    #define PIN_SWCLK 20
    #define PIN_RESET 18 -- active low
    #define PIN_POWER 22 -- active low
    Note: in it's current state power is active low, because it has been designed to control a P-channel mosfet.
    I used a AO3401A P-channel mosfet with a couple hundred ohm resistor in series on the gate to protect the
    Teensy GPIOs.

* Note: if you are extracting directly off of a full circuit board, the bulk capacitance might mean,
that turning off the power will take a long time, and if that happens swdFirmwareExtractor will not
work out of the box. You have a couple options:
  1) Add a delay after ```calling targetSysOff();```. I had to add ```waitms(30u);```. This will slow
     down firmware extraction. On a teensy3.2 with 30ms power off delay, it takes a little less then
     an hour to extract 64 kilobytes of firmware.
  2) Add a discharge N-channel MOSFET, something like AO3400A. And edit the targetSysOff() code to
     turn on this mosfet when powering off.
  2) Remove the capacitors
  3) Remove the chip
