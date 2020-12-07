// Control logic and initilazation
// Author: Erik Hedlund
// Author: Tobias Vinsa
// Version: 2020-12-06

#include <stdint.h>
#include <pic32mx.h>
#include "snakefuncs.h" 

  //Initilazing relevant ports for I/O 
void control_init(void)
{ //TODO add initilization of BTN1
  volatile int * MyTRISD = (int *) TRISD; 
  *MyTRISD &= ~0xFE0;
}

int getsw( void ) {
    volatile int* myPORTD = PORTD;
    return (*myPORTD>>8)&0xF;
}

int getbtns(void) { //TODO Add BTN1
    volatile int* myPORTD = PORTD;
    return (*myPORTD>>4)&0xE;
}
