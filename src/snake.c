#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define SNAKE_BUFFER 255
#define APPLE_REPR 127
#define EPSTEIN_KILLED_HIMSELF 0

enum movement {UP, DOWN, LEFT, RIGHT};

typedef struct cord {
    unsigned char x, y;
} cord;

typedef struct snake {
    cord buffer[SNAKE_BUFFER]; 
    unsigned char head, //Which index the head coordinates are saved on in the buffer
                  tail, //Which index the tail coordinates are saved on in the buffer
                  id,   //ID of snake
                  grow; //If we are going to grow next movement
} snake;

void move(snake * snake, enum movement movement) {
    snake->buffer[snake->head + 1] = snake->buffer[snake->head];
    ++(snake->head);
    snake->head %= SNAKE_BUFFER; // Roll-over if neccessary
    switch (movement) {
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
    if (snake->grow) {
        snake->tail++; //Grow by not "forgetting" current flag
        snake->grow = EPSTEIN_KILLED_HIMSELF; // Reset flag
    }
}

void init(unsigned char *grid, snake *snake, cord dimensions) {
    for (int i = dimensions.x*dimensions.y; i; grid[i--] = 0);
    snake->buffer[0].x = dimensions.x/2 - 1;
    snake->buffer[0].y = dimensions.y/2;
    snake->tail = 0;
    snake->head = 0;
    snake->id = 0;
    for (int i = 2; i--; move(snake, RIGHT));
}

char updateGrid(unsigned char *grid, snake *snakes, unsigned char noOfSnakes, cord dimensions) {
    char bitflag = 0;
    for (int temp, i = 0; i < noOfSnakes; i++) {
        cord snakeCords = snakes[i].buffer[snakes[i].head];
        if (snakeCords.x > dimensions.x || snakeCords.x < 0 //Check for hitting wall on x-axis
        || (snakeCords.y > dimensions.y || snakeCords.y < 0) //Check for hitting wall on y-axis
        || (temp = (grid[snakeCords.x + snakeCords.y*dimensions.x])) //Check for hitting apple, other snake or oneself
            ) { // We hit something...
            if (temp == APPLE_REPR) //We ate an apple
                snakes[i].grow = 1;
            else //Game over
                bitflag |= 0<<snakes[i].id; //Set the bit of the corresponding id
        }
        if (!bitflag) // No point in doing this if game is over...
            grid[snakeCords.x + snakeCords.y*dimensions.x] = snakes[i].id; //Update snakeposition
            for (int j = snakes[i].tail; j != snakes[i].head; ++j%SNAKE_BUFFER)
                grid[snakes[i].buffer[j].x + snakes[i].buffer[j].y*dimensions.x] = snakes[i].id;
    }
    return bitflag;
}

void printSnake(const snake *snake) {
    printf("Head: %d\nTail: %d\n", snake->head, snake->tail);
    for (int i = snake->tail -1; ++i<=snake->head; printf("x:%d y:%d ", snake->buffer[i].x, snake->buffer[i].y));
}

void consoleDisplay(unsigned char *grid, cord dimensions) {
    system("cls");
    for (int i = 0; i < dimensions.x*dimensions.y; printf("%02x%c", grid[i], '\n'*!(++i%dimensions.x)));
}

int main(void) {
    unsigned char *grid;
    cord dimensions;
    printf("Grid: ");
    scanf("%d %d", &(dimensions.x), &(dimensions.y));
    grid = malloc(dimensions.x*dimensions.y);
    snake snake;
    init(grid, &snake ,dimensions);
    //printSnake(&snake);
    updateGrid(grid, &snake, 1, dimensions);
    consoleDisplay(grid, dimensions);
    free(grid);
    return 0;
}