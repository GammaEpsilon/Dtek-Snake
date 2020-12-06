#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snake.h"  /* Declatations for these labs */

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define highscores int[5];

//Displays the current highscores and the players result this game
void highscore(snakelength) {
    //Fix a static highscore image
    //skapa en array som håller highscore värden och sedan presenta top 3/5 etc. Kolla sedan om det sista elementet på 
        int i = 0 //säg att 5 platser visas på vårt highscore bord
        for(i;i < 5; i++) {
            if(highscores[i] != NULL) {
                if(snakelength > highscores[i]) {
                    int tempscore = highscores[i];
                    highscores[i] = snakelength;
                    highscore(tempscore) //skicka rekursvit in tempscore för att byta ut de andra highscorsen när vi byter ut ett tidigare i listan
                }
            }
			else {
            highscores[i] = snakelength;
            break;
			}
        }
    display_highscore(3, highscores);
    display_update();
}

void display_highscore(int line, int[] *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}