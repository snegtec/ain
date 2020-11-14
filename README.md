/*****************************************************************************
*
* AIN Led Module
* a simple led module with a microcontroller for Petzl Duo Led headlamp
* 
* Note: This is just a hobby project. I am not connected in any way with 
* Petzl company.
* 
* version: 0.1 (November 2020)
* author: Erik Kos
*          snegtec.com
*          snegtec@outlook.com
* 
* Copyright (C) 2014-2016 Erik Kos
* 
* License:
*   application, source code - GNU General Public License
*   hardware (schematics, pcb design) - Creative Commons Attribution 
*   Share-Alike license (CC BY-SA 4.0)
* 
****************************************************************************/


Used tools
* compiler: Atmel Studio 6
* application for pcb design: Eagle 6.5.0.

Notes/problems/bugs/limitations:
* This is very simple module. Don't expect exciting features.
* One 7135 driver is enough for one side FR4 pcb.
* There has to be a schottky diode. Standard diode disables change of modes when battery is out. If you want to use BAT54C SOT23 package (or similar) you have to change the pcb.
* It is enough to give 10uF ceramic capacitor. Memory is back up well - ATTiny takes below 1uA in power down state.
* ATtiny13 has too small flash memory to be good for such project. If you want to add some features - use ATMega328 or similar.
* The pcb size is an approximation. Check the size and alignment of power pins before ordering a pcb.
* Soldering pins for ISP may be tricky.

time to discharge (to 0,9v per accu, approximatelly): 
* main mode: 3h 
* 1/2 mode: 6h
* 1/4 mode: 12h
* dim mode: ?h


/****************************************************************************/

