// Control logic and initilazation
// Author: Erik Hedlund
// Author: Tobias Vinsa
// Version: 2020-12-06

#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#include "clock.h"

void *stdin, *stdout, *stderr;

char timeOutCounter;

char count = 0;

//Slightly modified code from lab 3
//Based on example code 14-4
void clockinit(char timer) {
    count = 0;
    timeOutCounter = timer;
    T2CON &= ~0x8000; //Stanna klockjävel 2
    //T3CON &= ~0x8000; //Stanna klockjävel 3
    T2CONSET = 0x70; //Sätt PreScaling till 256 och kombinera klocka 2 och 3
    TMR2 = 0; //Initialisera till 0
    PR2 = (80000000/256)/10; //Sätt till korrekt värde för 100ms timeout 0x3D
    IFS(0) = 0; //Reset:a timeoutflag, bara för att vara säker...
    T2CON = 0x8070; //Starta klockjäveln
}

//Will wait until time interval specified in clockinit has passed since last clockinit or wait call
void wait(void) {
    do {
        while(!(IFS(0)) & 0x100); //Wait for timeout
        //T2CON = 0x70; //Stanna klockjäveln
        IFS(0) = 0; //Reset:a timeoutflag
        //T2CON = 0x8070; //Starta klockjäveln
        count++;
        count%=timeOutCounter;
    }
    while(count);
}