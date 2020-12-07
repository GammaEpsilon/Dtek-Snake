#ifndef CLOCK_H
#define CLOCK_H

enum mode {SECOND, HALFSECOND};
//enum mode: timeout interval, HALFSECOND or SECOND
void clockinit(enum mode);
//Will wait until time interval specified in clockinit has passed since last clockinit or wait call
void wait(void);

#endif