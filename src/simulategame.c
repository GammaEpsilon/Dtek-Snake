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
    unsigned char buffer[30*30], gameover;
    game_init(2, 30, 30, buffer);
    enum movement moves[][2] = {
        {RIGHT, RIGHT},
        {OLD, OLD},
        {OLD, OLD},
        {DOWN, UP},
        {OLD, OLD}
    };
    int i = 0;
    for (char gameover; 1;) { 
        gameover = turn(moves[i++%5], buffer);
        if (!gameover) {
            sleep((DWORD)1);
            consoleDisplay(buffer, 30, 30);
        } else break;
    };
    puts(buffer);
    return 0;
}

int player_game(void) {
    int x = 15;
    unsigned char buff[x*x];
    game_init(1, x, x, buff);
    enum movement map['w'+1][1];
    for (char i = 0; i <= 'w'; map[i++][0] = OLD);
    map['w'][0] = UP;
    map['a'][0] = LEFT;
    map['s'][0] = DOWN;
    map['d'][0] = RIGHT;
    while (1) {
        for (char c; (c = getchar()) != '\n'; consoleDisplay(buff, x, x))
            if (turn(map[c], buff)) {
                puts(buff);
                return 0;
            }
    }
}

int main(void) {
    return player_game();
}