#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "snake.h"

void consoleDisplay(const unsigned char *grid, int x, int y) {
    system("cls");
    for (int i = 0; i < x*y; printf("%02x%c", -grid[i]&0xff, '\n'*!(++i%x)));
}

int automatic_game(void) {
    int x = 15;
    unsigned char buffer[x*x];
    game_init(AVERAGE, x, x, buffer, 1);
    enum movement moves[] = {
        RIGHT,
        OLD,
        OLD,
        DOWN,
        OLD
    };
    int i = 0;
    for (char gameover; 1;) { 
        gameover = turn(moves[i++%5], buffer);
        if (!gameover) {
            sleep((DWORD)1);
            consoleDisplay(buffer, x, x);
        } else break;
    };
    puts((char *)buffer);
    return 0;
}

int player_game(void) {
    int x = 15;
    unsigned char buff[x*x];
    game_init(RETARDED, x, x, buff, 1);
    enum movement map['w'+1];
    for (char i = 0; i <= 'w'; map[i++] = OLD);
    map['w'] = UP;
    map['a'] = LEFT;
    map['s'] = DOWN;
    map['d'] = RIGHT;
    while (1) {
        for (char c; (c = getchar()) != '\n'; consoleDisplay(buff, x, x))
            if (turn(map[c], buff)) {
                puts(buff);
                return 0;
            }
    }
}

int main(void) {
    return automatic_game();
}