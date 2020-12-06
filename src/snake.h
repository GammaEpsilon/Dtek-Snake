#ifndef SNAKE_H
#define SNAKE_H

enum movement {OLD ,UP, DOWN, LEFT, RIGHT};

enum AI {NONE, BRAINDEAD, RETARDED, AVERAGE};

/*Params
 *No of snakes in game
 *x dimension
 *y dimension
 *output
 *returns 0 on fail and 1 on success
 */
char game_init(enum AI, int, int, unsigned char*, char, unsigned int);
/*
 * Arrays with each respective snakes movement
 * output array
 * returns 1 on game over
 */
char turn(enum movement, unsigned char*);

#endif