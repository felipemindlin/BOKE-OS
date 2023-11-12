#ifndef PONG_H
#define PONG_H

#include <stdint.h>
#include "colors.h"
//#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1024



#define SCORE_X (SCREEN_WIDTH / 2 - 10)
#define SCORE_Y (BORDER_SIZE + 10)
#define SCORE_WIDTH 20
#define SCORE_HEIGHT 20

#define CHARACTER_WIDTH 5
#define CHARACTER_HEIGHT 8
#define SCORE_PADDING 10
#define UP 1
#define DOWN 0
#define STOP 2

#define SCORE_CARD_X 385
#define SCORE_CARD_Y 10
#define SCORE_CARD_FONT_SIZE 4

#define BENCHMARK_UPPER_BOUND 1000000000
#define SLEEP_BENCHMARK_UPPER_BOUND 70000000

#define DEFAULT_FONT_SIZE 1


typedef struct {
    int x;
    int y;
    int width;
    int height;
    int speed;
    char direction;
    uint64_t color;
} Paddle;

typedef struct {
    int x;
    int y;
    int size;
    int speedX;
    int speedY;
    uint64_t color;
} Ball;

void set_paddle(Paddle *paddle, int x, int y, int width, int height, int speed, char directon, uint64_t color); //setea los valores de la estructura paddle
void draw_paddle(Paddle* paddle, uint64_t color); //dibuja el paddle
void clear_paddle(Paddle* paddle); //borra el paddle
void move_paddle_up(Paddle* paddle); //mueve el paddle hacia arriba
void move_paddle_down(Paddle* paddle); //mueve el paddle hacia abajo
void set_ball(int x, int y, int size, int speedX, int speedY, uint64_t color); //setea los valores de la estructura ball
void move_ball(); //mueve la pelota
void draw_borders(); //dibuja los bordes
void pong(); //funcion principal del juego pong
void draw_middle_line(); //dibuja la linea del medio
void resetGame(); //resetea el juego
void * get_pong();

#endif /* PONG_H */