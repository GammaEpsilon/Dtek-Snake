// Main function of snake game
// Author: Erik Hedlund
// Version: 2020-12-06
#include <stdint.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "IOcontrols.h"
#include "snake.h"
#include "clock.h"
#include "snakefuncs.h"
#include "highscoredisplay.h"

//#define wait(count) {int x = count ; do { while(!((IFS(0)) & 0x100)) IFS(0) &= ~0x100; } while(--x);}

#define cleartext() {int i = 3; while(i>-1) display_string(i--, ""); display_update();}
//#define wait() { while(!((IFS(0)) & 0x100)) IFS(0) &= ~0x100;}
enum state {MENU, GAME, SCOREBOARD, ERROR};

void *stdin, *stdout, *stderr;

volatile int* currentClock = 0;

unsigned int timeinmenu = 0;
unsigned int states = 0;

int* returnRand(void) {
    return currentClock;
}

void program_init() {
    //All program initlization goes here
    control_init();
    lab_init();
}

void display(const unsigned char *grid) {
    static unsigned char reference[(32*128)/8];
    unsigned char buffer[(32*128)/8];
    int i, j, k;
    for(i = 0; i<4; i++)
        for(j = 0; j<128; j++) {
            unsigned char input = 0;
            for (k=0; k<8; k++)
                input |= (grid[128*8*i+128*k+j] != 0)<<k;
            /*if (input != reference[128*i+j]) {
                display_changepage(input, i, k);
                reference[128*i+j] = input;
            }*/
            buffer[128*i+j] = input;
        }
    display_entire_oled(buffer);
}

//This is a rather dirty solution, but ISF(0) Just refuses to work
dirty_wait(int count) {
    do {
    currentClock = (int*) TMR2;
    T2CON = 0; //Stop the clock
    T2CONSET = 0x70; //Initiate PreScaling to 256 
    TMR2 = 0;
    PR2 = 0xffffffff; //Set to max
    T2CONSET = 0x8070;
    while (TMR2 < (80000000/256)/10);
    } while(--count);
}
//Main manu of the game, leads to a new game or displays the scoreboard
enum state menu() {
    T2CON = 0; //Stanna klockjäveln
    T2CONSET = 0x70; //Sätt PreScaling till 256 
    TMR2 = 0;
    T2CONSET = 0x8070;
    display_string(0, "Welcome to");
	display_string(1, "Hedlund's");
	display_string(2, "and Vinsa's");
	display_string(3, "snake game!");
    display_update();
    int buttons, i;
    enum state exitpoints[] = {GAME, SCOREBOARD};
    dirty_wait(5);
    while (!((buttons = getbtns())&0x3)) {
        timeinmenu += TMR2/10;
        TMR2 = 0;
    }
    for (i = 1; i <= 2; i++)
        if (buttons&i)
            return exitpoints[i-1];
}
//Edits the scoreboard with the players end score and username input
void edit_scoreboard(unsigned int score) {
    int buttons;
    unsigned char buff[3] = {0, 0, 0}; 
    char textrep[4] = "aaa";
    char index, i;
    buff[3] = '\0';
    display_string(0, "Enter name:");
    display_string(1, textrep);
    display_update();
    for (index = 0; index < 4; buttons = getbtns())
        if (buttons) {
            if (buttons&3) {
                buff[index] += (buttons>>1)&1 - (buttons&1);
                buff[index] %= ('z' - 'a'); // Roll over if neccessary
                for (i = 0; i < 3; i++) textrep[i] = buff[i] + 'a';
                display_string(1, textrep);
                display_update();
            }
            if (buttons&0xc) {
                if (buttons&8 && index) //If index = 0 we are going to have a bad time, m'key
                    index--;
                if (buttons&4)
                    index++;
            }
            dirty_wait(5);
        }
    // Insert into highscoretable if valid score
    enter_highscore(score, textrep);
}
// Initiates the game
enum state game() {
    const int x = 128;
    const int y = 32;
    int returnVal;
    unsigned int i, buttons;
    unsigned char grid[x*y];
    int switches = getsw(); // Switch 1 and 2 determines AI level, switch 3 determines structures
    unsigned int seed = timeinmenu|states; //Should be random enough
    if (! game_init(switches&0x3, x, y, grid, switches&0x4, seed)) return ERROR; //Something went wrong...
    do {
        enum movement move, controlMap[] = {DOWN, RIGHT, UP, LEFT}; // Map index corresponds to button id-1, lower index gives priority
        if (!(buttons = getbtns()))
            move = OLD; //Set old if no new input at timeout
        else
            for (i = 0; i < 4; i++)
                if (buttons&(1<<i)) {
                    move = controlMap[i];
                    break;
                }
        dirty_wait(5>>((switches>>3)&1));
        returnVal = turn(move, grid);
        display(grid);
    } while (returnVal >= 0); // removed earlier ! returnVal&~0x80000000
    if (switches&0x3) { // If multiplayer
        seed = 0; // No need to preserve seed anymore
        int i;
        for (i = 0; i < 2; i++)
            if ((1<<i)&returnVal) {
                char temp[] = "Player   lost!";
                temp[7] = i + '0';
                display_string(seed, temp);
                seed++;
            }
        display_string(seed++, "Press button");
        display_string(seed,  "to continue");
        display_update();
        dirty_wait(5);
        while (!getbtns());
        return MENU;
        } else {
            if (wouldgetin(returnVal&~0x80000000))
                edit_scoreboard(returnVal&~0x80000000);
            return SCOREBOARD;
        }   
}

//State for displaying the scoreboard
enum state view_scoreboard() {
    enum state exitpoints[] = {MENU};
    int buttons = 0;
    int i = 1;
    display_highscores();
    dirty_wait(5);
    while (!((buttons = getbtns())&0x4)); // Maybe sleep to ease up performance
        if (buttons&i)
            return exitpoints[i-1];
}

// What if something goes wrong
enum state error() {
    display_string(0, "ERROR");
    display_update();
    while(1); // Sleep forever
}

// Start the program
int main(void) {
    int i,j;
    enum state state = MENU;
    program_init();
    dirty_wait(1);
    while (1500) {
        states++;
        cleartext();
        switch(state) { //Our fancy state machine
            case MENU:
                state = menu();
                break;
            case GAME:
                state = game();
                break;
            case SCOREBOARD:
                state = view_scoreboard();
                break;
            case ERROR:
                state = error();
                break;
        }
    }
}