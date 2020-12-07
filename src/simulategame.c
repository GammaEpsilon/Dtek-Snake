
#ifdef _WIN32
#include <stdio.h>
#include <Windows.h>

#include "snake.h"


void consoleDisplay(const unsigned char *grid, int x, int y) {
    system("cls");
    int i;
    for (i = 0; i < x*y; printf("%02x%c", -grid[i]&0xff, '\n'*!(++i%x)));
}

int automatic_game(void) {
    int x = 20;
    unsigned char buffer[x*x];
    game_init(NONE, x, x, buffer, 0, 0);
    enum movement moves[] = {
        RIGHT,
        OLD,
        OLD,
        DOWN,
        OLD
    };
    int i = 0;
    for (int gameover; 1;) { 
        gameover = turn(moves[i++%5], buffer);
        if (gameover >= 0) {
            sleep((DWORD)1);
            consoleDisplay(buffer, x, x);
        } else break;
    };
    puts((char *)buffer);
    return 0;
}

int player_game(void) {
    int x = 128;
    int y = 32;
    enum AI ai = NONE;
    unsigned char buff[x*y];
    game_init(ai, x, y, buff, 0, 0);
    enum movement map['w'+1];
    for (char i = 0; i <= 'w'; map[i++] = OLD);
    map['w'] = UP;
    map['a'] = LEFT;
    map['s'] = DOWN;
    map['d'] = RIGHT;
    for (int gameover; 1;) {
        for (char c; (c = getchar()) != '\n'; consoleDisplay(buff, x, y))
            if ((gameover = turn(map[c], buff)) < 0) {
                if (ai) {
                    for (int i = 0; i <= 2; i++)
                        if ((1<<i)&gameover)
                            printf("Player %d lost\n", i);
                } else  printf("Final score: %d", gameover&~0x80000000);
                return 0;
            }
    }
}

int main(void) {
    return player_game();
}
#endif