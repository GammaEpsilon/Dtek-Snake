
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#include "clock.h"

//Slightly modified code from lab 3
//Based on example code 14-4
void clockinit(enum mode mode) {
    volatile int * myIFS0 = IFS(0);
    T2CON &= ~0x8000; //Stanna klockjävel 2
    T3CON &= ~0x8000; //Stanna klockjävel 3
    T2CONSET |= 0x78; //Sätt PreScaling till 256 och kombinera klocka 2 och 3
    TMR2 = 0; //Initialisera till 0
    switch (mode) {
        case HALFSECOND:
            PR2 = (80000000/256)/2; //Sätt till korrekt värde för 500ms timeout
            break;
        case SECOND:
            PR2 = 80000000/256; //Sätt till korrekt värde för 1s timeout
            break;
    }
    *myIFS0 &= ~0x100; //Reset:a timeoutflag, bara för att vara säker...
    T2CON |= 0x8000; //Starta klockjäveln
}

//Will wait until time interval specified in clockinit has passed since last clockinit or wait call
void wait(void) {
    volatile int * myIFS0 = IFS(0);
    while(!myIFS0&0x100); //Wait for timeout
    T2CON &= ~0x8000; //Stanna klockjäveln
    *myIFS0 &= ~0x100; //Reset:a timeoutflag
    T2CON |= 0x8000; //Starta klockjäveln
}