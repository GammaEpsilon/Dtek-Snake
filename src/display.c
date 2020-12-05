#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snake.h"  /* Declaarations for this project */

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

/* Initilazing relevant ports for I/O */
void labinit( void )
{
  volatile int * MyTRISE = (int *) 0x1F886100;
  *MyTRISE |= 0xFF;
  volatile int * MyTRISD = (int *) TRISD; 
  *MyTRISD &= ~0xFE0;
}

/* This function is called repetitively from the main program to update the display for the game */
void display( void )
{
  int buttons, switches, i;
  //Check button inputs to swap direction of snake1
  if (buttons = getbtns()) {
    for (i = 0; i<3; i++) 
      if (buttons&(1<<i)) {
          buttons<<((i+1)*4)
      }
  }
  //Check swtich inputs to swap direction of snake2
  if (switches = getsw()) {
    for (i = 0; i<3; i++) 
      if (switches&(1<<i)) {
            switches<<((i+1)*4)
      }
  }

  delay( 1000 );
  display_update(gameMap); //alter to the game map.
}

void display_update(unsigned char [][]gameMap) {
	int i, j, k;
    char c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = gameMap[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}