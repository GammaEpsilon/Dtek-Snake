/* highscoredisplay.c

   This file written 2020 by T Vinsa and E Hedlund

   Some parts are original code written by F Lundevall 
   Updated 2017-04-21 by F Lundevall

   Latest update 2020-12-06 by T Vinsa and E Hedlund

   For copyright and licensing, see file COPYING */


#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snakefuncs.h"  /* Declaratations for the game */

void *stdin, *stdout, *stderr;

typedef struct score {
  char name[4];
  int score;
} score;

#define NEW_SCORE { "nul", 0 }

score highscores[3] = {NEW_SCORE, NEW_SCORE, NEW_SCORE};

/* Interrupt Service Routine */
void user_isr( void ) //Den här är väl helt onödig?
{
  return;
}

// Displays the current highscores
void testinghighscore(int line, score *s) {
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
			
  strcpy(textbuffer[line], s->name);
	textbuffer[line][4] = ' ';
  char buff[10];
  itoa(s->score, buff);
  strcpy(textbuffer[line] + 4, buff);
  textbuffer[line][3] = ' '; //Insertspace
  int i = 5;
  for(i; i<16;i++) textbuffer[line][i] = ' ';
}

// Updates the highscore if a new highscore is reached. Maximum 3 highscore positions
void enter_highscore(int snakelength, char* name) {
	int i;
	for(i=0;i < 3; i++) {
    if(snakelength > highscores[i].score) {
      char tempName[4];
      int tempscore = highscores[i].score;
      strcpy(tempName, highscores[i].name);
      highscores[i].score = snakelength;
			strcpy(highscores[i].name, name);
      enter_highscore(tempscore, tempName);
      break;
    }
	}
  return;
}
// Displays the top players and their highscore during this run
void display_highscores(void) {
	display_string(0, "Scores!:");
	testinghighscore(1, highscores);
	testinghighscore(2, highscores+1);
	testinghighscore(3, highscores+2);
	display_update();
  return;
}

// Returns 1 if x > lowest highscore, 0 otherwise
char wouldgetin(int x) {
  if(x > highscores[2].score) return 1;
  return 0;
}