#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define SNAKE_BUFFER 255
#define APPLE_REPR 127
#define EPSTEIN_KILLED_HIMSELF 0
#define reset_grid(grid, dimensions) for (int i = dimensions.x*dimensions.y; i; grid[i--] = 0)
#define generate_apple(grid, dimensions) { do { appleLoc = rand()%(dimensions.x*dimensions.y); } while (grid[appleLoc]); grid[appleLoc] = APPLE_REPR; }

//TODO Remove
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "snake.h"

typedef struct cord {
    unsigned char x, y;
} cord;

typedef struct snake {
    cord buffer[SNAKE_BUFFER]; 
    unsigned char head, //Which index the head coordinates are saved on in the buffer
                  tail, //Which index the tail coordinates are saved on in the buffer
                  id,   //ID of snake
                  grow; //If we are going to grow next movement
    enum movement movement;
} snake;

snake *snakes;
char noOfS;
int appleLoc;
cord dims;

void move(snake * snake) {
    snake->buffer[snake->head + 1] = snake->buffer[snake->head];
    ++(snake->head);
    snake->head %= SNAKE_BUFFER; // Roll-over if neccessary
    switch (snake->movement) {
        case UP:
            snake->buffer[snake->head].y--;
            break;
        case DOWN:
            snake->buffer[snake->head].y++;
            break;
        case LEFT:
            snake->buffer[snake->head].x--;
            break;
        case RIGHT:
            snake->buffer[snake->head].x++;
            break;
    }
    if (!(snake->grow)) {
        snake->tail++; // Do not grow by updating tail index
        snake->tail %= SNAKE_BUFFER; // Roll-over if neccessary
    }
    else    
        snake->grow = EPSTEIN_KILLED_HIMSELF; // We have grown, time to reset flag
    }

void init(snake *snake, cord dimensions) {
    static char createdSnakes = 1.0;
    snake->buffer[0].x = dimensions.x/2 - 1;
    snake->buffer[0].y = (int) (dimensions.y * (createdSnakes/(noOfS+1.0)));
    snake->tail = 0;
    snake->head = 0;
    snake->id = createdSnakes++;
    snake->movement = RIGHT;
    for (int i = 1; i--; snake->grow=1) move(snake); //Set initial size to 3
}

// Returns != 0 when game over
char updateGrid(unsigned char *grid, snake *snakes, unsigned char noOfSnakes, cord dimensions) {
    char bitflag = 0;
    for (int temp, i = 0; i < noOfSnakes; i++) {
        cord snakeCords = snakes[i].buffer[snakes[i].head];
        if (snakeCords.x > dimensions.x || snakeCords.x < 0 //Check for hitting wall on x-axis
        || (snakeCords.y > dimensions.y || snakeCords.y < 0) //Check for hitting wall on y-axis
        || (temp = (grid[snakeCords.x + snakeCords.y*dimensions.x])) //Check for hitting apple, other snake or oneself
            ) // We hit something...
            if (temp == APPLE_REPR) {
                snakes[i].grow = 1; //We ate an apple
                generate_apple(grid, dimensions);
                printf("%d\n", appleLoc);
            }
            else //Game over
                bitflag |= (1<<snakes[i].id); //Set the bit of the corresponding id
        if (!bitflag) { // No point in doing this if game is over...
            if (!i) //Only do this the first iteraiton
                reset_grid(grid, dimensions); //Reset grid
            grid[snakeCords.x + snakeCords.y*dimensions.x] = snakes[i].id; //Update snakeposition
            for (int j = snakes[i].tail; j != snakes[i].head; ++j%SNAKE_BUFFER)
                grid[snakes[i].buffer[j].x + snakes[i].buffer[j].y*dimensions.x] = snakes[i].id;
        }
    }
    grid[appleLoc] = APPLE_REPR; //Place down apple
    return bitflag;
}

void printSnake(const snake *snake) {
    printf("Head: %d\nTail: %d\n", snake->head, snake->tail);
    for (int i = snake->tail -1; ++i<=snake->head; printf("x:%d y:%d ", snake->buffer[i].x, snake->buffer[i].y));
}

/*
int main(void) {
    snake snake;
    cord dimensions = {25, 25};
    unsigned char * grid = (unsigned char *)malloc(dimensions.x*dimensions.y);
    reset_grid(grid, dimensions);
    init(&snake, dimensions);
    enum movement moves[] = {
        RIGHT,
        RIGHT,
        RIGHT,
        UP,
        UP
    };
    char bitflag;
    for (int i = 0; !(bitflag = updateGrid(grid, &snake, 1, dimensions)); i++) {
        consoleDisplay(grid, dimensions);
        snake.movement = moves[i%5];
        move(&snake);
        Sleep((DWORD) 250);
    }
    for (char i = 0; i < 8; i++) {
        if ((1<<i)&bitflag)
        printf("Game over, player %d lost", i);
    }
    free(grid);
}
*/
void game_init(char noOfSnakes, int x, int y, unsigned char *grid) {
    int apple;
    snakes = (snake*)malloc(noOfSnakes*sizeof(snake));
    dims.x = x;
    dims.y = y;
    noOfS = noOfSnakes;
    reset_grid(grid, dims);
    for (int i = 0; i < noOfS; i++) init(snakes + i, dims);
    generate_apple(grid, dims);
}

char turn(enum movement *movement, unsigned char * output) {
    int bitflag;
    for (int i = 0; i < noOfS; i++) {
        if (movement[i])
            snakes[i].movement = movement[i];
        move(snakes + i);
    }
    if (!(bitflag = updateGrid(output, snakes, noOfS, dims))) { //Not gameover
        return 0;
    } else {
        int i;
        int index = 0;
        for (char i = 0; i < 8; i++) {
            if ((1<<i)&bitflag)
                index += sprintf(output + index, "Game over, player %d lost\n", i);
        }//Clean up
        //free(grid);
        free(snakes);
        return 1;
    }
}