/* Snake game for MS-DOS 6.22       */
/* Turbo C / Watcom      (ikr old)  */
/* Mode 13h VGA - 320x200x256       */
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 320
#define HEIGHT 200
#define CELL 10
#define MAX_LEN 100

unsigned char far* vga = (unsigned char far*)MK_FP(0xA000, 0x0000);

int snake_x[MAX_LEN], snake_y[MAX_LEN];
int length = 5;
int dir = 0; // 0=right,1=down,2=left,3=up
int food_x, food_y; // for apple
int food_id = 0;
int gameover = 0;
int score = 0;

// BIOS drawing functions and stuff
void set_mode13h() {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = 0x13;
    int86(0x10, &regs, &regs);
}

void set_text_mode() {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = 0x03;
    int86(0x10, &regs, &regs);
}

void draw_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        vga[y * WIDTH + x] = color;
}

void draw_block(int x, int y, unsigned char color) {
    for (int i = 0; i < CELL; i++) {
        for (int j = 0; j < CELL; j++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

int get_colf() {
    switch(food_id) { // return the dos color code
       case 0: return 0x4; break;
       case 1: return 0xE; break;
       case 2: return 0xC; break;
       case 3: return 0xD; break;
       case 4: return 0x6; break;
    }
}
void generate_food() { // place apples
    food_x = (rand() % (WIDTH / CELL)) * CELL;
    food_y = (rand() % (HEIGHT / CELL)) * CELL;
    draw_block(food_x, food_y, get_colf());
}

void init_snake() {
    int startx = 50;
    int starty = 50;
    for (int i = 0; i < length; i++) {
        snake_x[i] = startx - i * CELL;
        snake_y[i] = starty;
    }
}

void draw_snake() { // draw the blocks of the snakes location
    for (int i = 0; i < length; i++) {
        draw_block(snake_x[i], snake_y[i], 10);
    }
}

void clear_tail(int x, int y) {
    draw_block(x, y, 0);
}

void move_snake() { // movement and collision
    // clear tail
    clear_tail(snake_x[length - 1], snake_y[length - 1]);

    for (int i = length - 1; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    switch (dir) {
        case 0: snake_x[0] += CELL; break;
        case 1: snake_y[0] += CELL; break;
        case 2: snake_x[0] -= CELL; break;
        case 3: snake_y[0] -= CELL; break;
    }

    // check border collision
    if (snake_x[0] < 0 || snake_x[0] >= WIDTH ||
        snake_y[0] < 0 || snake_y[0] >= HEIGHT) {
        gameover = 1;
    }

    // check self collision
    for (int i = 1; i < length; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
            gameover = 1;
        }
    }

    // check food collision
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        score+= ((1000*food_id) + (rand() % 1000));
        if (length < MAX_LEN) length++;
        generate_food();
    }
}

void delay_tick(unsigned int ticks) {
    delay(ticks); // Turbo C delay in ms
}

void read_input() {
    if (kbhit()) {
        char ch = getch();
        switch (ch) {
            case 27: gameover = 1; break; // ESC exit to dos
            case 'w': if (dir != 1) dir = 3; break;
            case 's': if (dir != 3) dir = 1; break;
            case 'a': if (dir != 0) dir = 2; break;
            case 'd': if (dir != 2) dir = 0; break;
        }
    }
}

int main() {
    system("cls");
    set_mode13h();
    srand(time(NULL));
    init_snake();
    generate_food();

    while (!gameover) {
        read_input();
        move_snake();
        draw_snake();
        delay_tick(100);
    }

    set_text_mode();
    printf("Game Over. Your score: %i ", score);

    return 0;


}
