/* snakefuncs.c

   This file written 2020 by T Vinsa and E Hedlund
   Some parts are original code written by A Isaksson and F Lundevall

   Latest update 2020-12-06 by T Vinsa and E Hedlund

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snakefuncs.h"  /* Declatations for these labs */
#include <stdint.h>

void *stdin, *stdout, *stderr;

/* Declare a helper function which is local to this file */
// Following code is from labs written by A Isaksson and F Lundevall:

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

// display_init func taken from the labs.
void display_init(void) {
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}
// display_string func taken from the labs.
void display_string(int line, char *s) {
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

// display_update func taken from the labs.
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

//Following is code written for the project;

//Used to display the entire oled screen
void display_entire_oled(const unsigned char *byte) {
    int i;
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x20);
	spi_send_recv(0x0);
    spi_send_recv(0x22);
    spi_send_recv(0x0);
    spi_send_recv(0x3);
    DISPLAY_CHANGE_TO_DATA_MODE;
    for (i = 0; i < 128*4; i++) {
        spi_send_recv(byte[i]);
    }
}

// Displays differnt parts of the oled given byte, page and column
void display_changepage(unsigned char byte, unsigned char page, unsigned char collum) {
	int i;
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x20);
    spi_send_recv(0x0);
    spi_send_recv(0x21);
    for (i = 0; i < 2; i++)
        spi_send_recv(collum); //Set correct collum
    spi_send_recv(0x22);
    for (i = 0; i < 2; i++)
        spi_send_recv(page); //Set correct page

    DISPLAY_CHANGE_TO_DATA_MODE;
    spi_send_recv(byte);
}

// Displays a single pixel change
void display_changepixel(int pixelIndex, unsigned char on, const unsigned char *grid) {
	int i;
	int page = pixelIndex/(128*8);
	int bit = (pixelIndex-page)/128;
	int collum = pixelIndex%128;
	char updateData = 0;
	for (i = 0; i < 8; i++) {
		if (grid[128*(page+i)+collum])
			updateData |= (1<<i);
		else
			updateData &= ~(1<<i);
	}
	if (on)
		updateData |= (1<<bit);
	else
		updateData &= ~(1<<bit);
	
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	spi_send_recv(0x20);
	for (i = 0; i < 2; i++)
		spi_send_recv(collum); //Set correct collum
	spi_send_recv(0x22);
	for (i = 0; i < 2; i++)
		spi_send_recv(page); //Set correct page

	DISPLAY_CHANGE_TO_DATA_MODE;
	spi_send_recv(updateData);
}

// Copies char into destination
int strcpy(char * dest, const char *src) {
int index;
char c;
for (index = 0; (c = src[index]) != '\0'; index++) dest[index] = c;
return index;
}

// Converts an int into a character array
void itoa(int x, char * buf) {
    int j, i;
    for (j = 10; x/j; j *= 10);
    j /= 10;
    for (i = 0; j; j /= 10) {
        buf[i++] = x/j + '0';
        x %= j;
    }
    buf[i] = '\0';
}


//Slightly modified main from time4int lab, written 2015 by Axel Isaksson, modified 2015, 2017 by F Lundevall
//modified sligthly 2020 by Erik Hedlund and Tobias Vinsa for this project
void lab_init(void) {
        /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
	//Removed a bunch of display_lines here
	display_init();
	display_update();
}
