#include <stdint.h>
#include <pic32mx.h>
#include "snake.h" 

int getsw( void ) {
    volatile int* myPORTD = PORTD;
    return (*myPORTD>>8)&0xF;
}

int getbtns(void) {
    volatile int* myPORTD = PORTD;
    return (*myPORTD>>5)&0x7;
}