#ifndef SNAKE_H
#define SNAKE_H

enum movement {OLD ,UP, DOWN, LEFT, RIGHT};

/*Params
 *No of snakes in game
 *x dimension
 *y dimension
 *output
 */
void game_init(char, int, int, unsigned char*);
/*
 * Arrays with each respective snakes movement
 * output array
 */
char turn(enum movement*, unsigned char*);

#endif