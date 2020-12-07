// Control logic and initilazation
// Author: Erik Hedlund
// Author: Tobias Vinsa
// Version: 2020-12-06

#include <stdint.h>
#include <pic32mx.h>
#include "snakefuncs.h" 

  //Initilazing relevant ports for I/O 
void control_init(void)
{ volatile int * MyTRISF = TRISF;
  volatile int * MyTRISD = (int *) TRISD; 
  *MyTRISF |= 0x2;
  *MyTRISD &= ~0xFE0;
}

int getsw( void ) {
    volatile int* myPORTD = PORTD;
    return (*myPORTD>>8)&0xF;
}

int getbtns(void) {
    volatile int* myPORTD = PORTD;
    return ((*myPORTD>>4)&0xE)|getbtn1();
}

int getbtn1(void) {
    volatile int* myPORTF = PORTF;
		return (*myPORTF>>1)&0x1;
}