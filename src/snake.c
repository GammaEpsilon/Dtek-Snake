#include <stdlib.h>

#define SNAKE_BUFFER 255
#define APPLE_REPR 127
#define MAX_SNAKES 2 //Increment if you want to increase the maximum supported snakes
#define EPSTEIN_KILLED_HIMSELF 0
#define reset_grid(grid, dimensions) { int i = dimensions.x*dimensions.y; while(i) grid[i] = resetgrid[i--]; }
#define generate_apple(grid, dimensions) { do { appleLoc = rand()%(dimensions.x*dimensions.y); } while (grid[appleLoc]); grid[appleLoc] = APPLE_REPR; }

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

snake snakes[MAX_SNAKES];
char noOfS, structures;
unsigned char resetgrid[128*32];
int appleLoc;
cord dims;
enum AI mode;

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
    int i;
    for (i = 1; i--; snake->grow=1) move(snake); //Set initial size to 3
}

void init_structgrid(cord dims, char multiplayer) {
    if (multiplayer) {
        cord arr[] = {{dims.x/4, dims.y/4}, {(int)((double) dims.x/4.0*3.0) + 1, dims.y/4}, {dims.x/4, (int)((double) dims.y/4.0*3.0 + 1)}, {(int)((double) dims.x/4.0*3.0 + 1), (int)((double) dims.y/4.0*3.0 + 1)}};
        int i, j;
        for (i = 0; i < 4; i++)
            for (j = -2; j <= 2; j++) { //Draw crosses
                resetgrid[(arr[i].y+j)*dims.x+arr[i].x] = 10;
                resetgrid[arr[i].y*dims.x+arr[i].x+j] = 10;
            }
        for (i = 4; i <= dims.x-4; resetgrid[dims.x*(dims.y/2) + (i++)] = 10);
    }
}

// Returns != 0 when game over
char updateGrid(unsigned char *grid, snake *snakes, unsigned char noOfSnakes, cord dimensions) {
    char bitflag = 0;
    int temp;
    int i;
    for (i = 0; i < noOfSnakes; i++) {
        cord snakeCords = snakes[i].buffer[snakes[i].head];
        if ((snakeCords.x > dimensions.x || snakeCords.x < 0) //Check for hitting wall on x-axis
        || (snakeCords.y > dimensions.y || snakeCords.y < 0) //Check for hitting wall on y-axis
        || (temp = (grid[snakeCords.x + snakeCords.y*dimensions.x])) //Check for hitting apple, other snake or oneself
            ) // We hit something...
            if (temp == APPLE_REPR) {
                snakes[i].grow = 1; //We ate an apple
                generate_apple(grid, dimensions);
            }
            else //Game over
                bitflag |= (1<<snakes[i].id); //Set the bit of the corresponding id
                //int length = (((snakes[i].head)-(snakes[i].tail)) + 1)
                //highscore(length); //Visa Highscore
        if (!bitflag) { // No point in doing this if game is over...
            if (!i) //Only do this the first iteraiton
                reset_grid(grid, dimensions); //Reset grid
            grid[snakeCords.x + snakeCords.y*dimensions.x] = snakes[i].id; //Update snakeposition
            int j;
            for (j = snakes[i].tail; j != snakes[i].head; ++j%SNAKE_BUFFER)
                grid[snakes[i].buffer[j].x + snakes[i].buffer[j].y*dimensions.x] = snakes[i].id;
            grid[appleLoc] = APPLE_REPR; //Place down apple
        }
    }
    return bitflag;
}

// "AI" Part

enum AI mode;

enum movement braindead(const snake *snake) {
    return snake->movement;
}
// 1/2 chance to OLD, 1/4 chance for the other two non conflicting directions
enum movement retarded(const snake *snake) {
    enum movement random = (rand()%4) + 1;
    return (
       ((random == UP || random == DOWN) && (snake->movement == UP || snake->movement == DOWN))
       ||
       ((random == LEFT || random == RIGHT) && (snake->movement == LEFT || snake->movement == RIGHT))
       ) ? snake->movement:random; //Return OLD if random chose same or conflicting direction, else random
}

enum movement average(const snake *snake, const unsigned char *grid) {
    cord applelocation = {appleLoc%dims.x, appleLoc/dims.x};
    cord snakeCord = snake->buffer[snake->head];
    // Following if statements works like this: Will I get closer to apple by going this way? If so; will I not crash? 
    if (applelocation.y > snakeCord.y && !grid[(snakeCord.y+1)*dims.x + snakeCord.x] || (snakeCord.y+1)*dims.x + snakeCord.x == appleLoc) return DOWN;
    if (applelocation.y < snakeCord.y && !grid[(snakeCord.y-1)*dims.x + snakeCord.x] || (snakeCord.y-1)*dims.x + snakeCord.x == appleLoc) return UP;
    if (applelocation.x > snakeCord.x && !grid[snakeCord.y*dims.x + snakeCord.x+1] || snakeCord.y*dims.x + snakeCord.x+1 == appleLoc) return RIGHT;
    if (applelocation.x < snakeCord.x && !grid[(snakeCord.y)*dims.x + snakeCord.x-1] || snakeCord.y*dims.x + snakeCord.x-1 == appleLoc) return LEFT;
    char i, temp[] = {-1, 1};
    for (i = 0; i < 4; i++) { // Just try not to fucking die
        if (! grid[(snakeCord.y+temp[i]*(i<3))*dims.x + snakeCord.x + temp[i]*(i>2)])
            return i + 1;
    }
    return (rand()%4)+1; //Fuck it
}

// Exposed functions

char game_init(enum AI ai, int x, int y, unsigned char *grid, char structures) {
    noOfS = 1 + !(!ai);
    mode = ai;
    if (noOfS > MAX_SNAKES) return 0; // Should never happen
    dims.x = x;
    dims.y = y;
    reset_grid(grid, dims);
    int i;
    for (i = 0; i < noOfS; i++) init(snakes + i, dims);
    if (structures)
        init_structgrid(dims, ai);
    generate_apple(grid, dims);
    return 1;
}

char turn(enum movement movement, unsigned char * output) {
    int bitflag;
    int j;
    if (movement)
        snakes[0].movement = movement;
    for (j = 0; j < noOfS; j++)
        move(snakes + j);
    if (!(bitflag = updateGrid(output, snakes, noOfS, dims))) { //Not gameover
        switch (mode) { // AI Switch
            case NONE:
                break;
            case BRAINDEAD:
                snakes[1].movement = braindead(snakes + 1);
                break;
            case RETARDED:
                snakes[1].movement = retarded(snakes + 1);
                break;
            case AVERAGE:
                snakes[1].movement = average(snakes + 1, output);
                break;
        }
        return 0;
    } else {
        char i, j, c;
        int index = 0;
        for (i = 0; i <= MAX_SNAKES; i++) {
            if ((1<<i)&bitflag) {
                char *temp = "Game over, player   lost\n";
                for (j = 0; (c = *temp++) != '\0'; j++) output[j+index] = c;
                output[18 + index] = '0' + i;
                index  += j;
            }
        }
        return 1;
    }
}
