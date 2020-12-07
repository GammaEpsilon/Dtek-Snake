#ifndef SNAKEFUNCS_H
#define SNAKEFUNCS_H

/* snakefuncs.h
   Header file for the snake game.
   This file modified 2020 by T Vinsa and E Hedlund
   Most parts are original code written by A Isaksson and F Lundevall in mipslabsfunc

   Latest update 2020-12-06 by T Vinsa and E Hedlund

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from snakefuncs.c */
void display_image(int x, const uint8_t *data);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
void lab_init(void);
void quicksleep(int cyc);

/* Declare lab-related functions from mipslabfunc.c */
void testHighscore(void);
void display_changepixel(int, unsigned char, const unsigned char*);
void itoa(int x, char * buf);
int strcpy(char * dest, const char *src);

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];
#endif