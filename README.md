# eurorack-clock-div-mulit

This code is still under development and has not been fully tested

 This code is for a module that provides a clock, a clock divider and a clock
 multiplier for modular synthesis. The module has two operating modes, Internl
 (the module provides its own clock signal) and External (the module relies on
 an external clock running at 24 pules per quarter note), and divides and 
 multiplies by 2, 4 and 8.

 Code is written for the Adafruit ItsyBitsy 32u4 5V.
 
 Pin assignments:
   4 - Switch for internal/external clock source
   6 - External clock input
   8 - Internal clock output
   5 - /8
   7 - /4
   9 - /2
  10 - x2
  11 - x4
  12 - x8
  A0 - Connected to 10k pot for internal clock rate

 Written by Scott Marler. GNU General Public License v3.0, check license.txt
 for more information.
