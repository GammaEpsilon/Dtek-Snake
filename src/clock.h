#ifndef CLOCK_H
#define CLOCK_H

//enum mode: timeout interval, HALFSECOND or SECOND
void clockinit(char);
//Will wait until time interval specified in clockinit has passed since last clockinit or wait call
void wait(void);

#endif