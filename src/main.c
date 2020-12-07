// Main function of snake game
// Author: Erik Hedlund
// Version: 2020-12-06
#include <stdint.h>
#include "IOcontrols.h"
#include "snake.h"
#include "clock.h"
#define cleartext() {int i = 3; while(i) display_string(i, "");}
enum state {MENU, GAME, SCOREBOARD, ERROR};

void *stdin, *stdout, *stderr;

void program_init() {
    //All program initlization goes here
    control_init();
}

enum state menu() {
    display_string(0, "Snake");
    display_string(1, "Btn1: Play Game");
    display_string(2, "Btn2: View Score");
    int buttons, i;
    enum state exitpoints[] = {GAME, SCOREBOARD};
    while (!((buttons = getbtns())&0x3)); // Maybe sleep to ease up performance
    for (i = 1; i <= 2; i++)
        if (buttons&i)
            return exitpoints[i-1];
}
enum state game() {
    const int x = 128;
    const int y = 32;
    unsigned int returnVal, i, buttons;
    unsigned char grid[x*y];
    int switches = getsw(); // Switch 1 and 2 determines AI level, switch 3 determines structures
    unsigned int seed = (int)&switches|(int)&returnVal; //Should be random enough
    if (! game_init(switches&0x3, x, y, grid, switches&0x4, seed)) return ERROR; //Something went wrong...
    initclock((switches>>3)&1);
    do {
        enum movement move, controlMap[] = {LEFT, UP, RIGHT, DOWN}; // Map index corresponds to button id-1, lower index gives priority
        if (!(buttons = getbtns()))
            move = OLD; //Set old if no new input at timeout
        else
            for (i = 0; i < 4; i++)
                if (buttons&(1<<i)) {
                    move = controlMap[i];
                    break;
                }
        returnVal = turn(move, grid);
        wait(); //Sleep twice as long if switch 4 is high
        //TODO Show grid
    } while (! returnVal&~0x80000000);
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
        display_string(seed, "Press button to continue");
        while (!getbtns());
        return MENU;
        } else {
            if (wouldgetin(returnVal&~0x80000000))
                edit_scoreboard(returnVal&~0x80000000);
            return SCOREBOARD;
        }
        
}
enum state view_scoreboard();
void edit_scoreboard(unsigned int score) {
    int buttons;
    unsigned char buff[3] = {0, 0, 0}; 
    char textrep[4] = "aaa";
    char index, i;
    buff[3] = '\0';
    display_string(0, "Enter name:");
    display_string(1, textrep);
    for (index = 0; index < 4; buttons = getbtns())
        if (buttons) {
            if (buttons&3) {
                buff[index] += (buttons>>1)&1 - (buttons&1);
                buff[index] %= ('z' - 'a'); // Roll over if neccessary
                for (i = 0; i < 3; i++) textrep[i] = buff[i] + 'a';
                display_string(1, textrep);
            }
            if (buttons&0xc) {
                if (buttons&4 && !index) //If index = 0 we are going to have a bad time, m'key
                    index--;
                if (buttons&8)
                    index++;
            }
        }
    //INSERT INTO SCOREBOARD(score, textrep)
    if(wouldgetin(score))
        enter_highscore(score, textrep);
    display_highscores();
}
enum state error() {
    display_string(0, "ERROR");
    while(1); // Sleep forever
}

int main(void) {
    enum state state = MENU;
    program_init();
    while (1500) {
        cleartext()
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