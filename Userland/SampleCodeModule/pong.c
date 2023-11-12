#include <unistd.h>
#include "include/pong.h"
#include "include/UserSyscalls.h"
#include "include/utils.h"
#include "include/sounds.h"
#include "colors.h"

unsigned int score =3;
unsigned int BORDER_SIZE = 20;

unsigned int PADDLE_WIDTH = 10;
unsigned int PADDLE_HEIGHT = 100;
unsigned int PADDLE_SPEED = 4;

unsigned int BALL_SIZE = 10;
unsigned int BALL_SPEED = 3;
unsigned int BALL_SPEED_HORIZONTAL = 6;
uint64_t BALL_COLOR = WHITE;
uint64_t PADDLE_COLOR = WHITE;
uint64_t BACKGROUND_COLOR = BLACK;
uint64_t BORDER_COLOR = WHITE;

uint64_t player1Up;
uint64_t player1Upbeak;
uint64_t player1Down;
uint64_t player1Downbeak;
uint64_t player2Up;
uint64_t player2Upbeak;
uint64_t player2Down;
uint64_t player2Downbeak;

int experimental = 0;

Paddle paddle1;
Paddle paddle2;
Ball ball;

int sound =1;
int gsound =1;
int player1Score = 0;
int player2Score = 0;
int movement_vector[] = {-1, 1, 0};

int asciiToKeyCode(char ascii){
    switch (ascii){
        case 0x1B: return 0x01;          case 0x08: return 0x0E;          case 0x09: return 0x0F;          case 0x0D: return 0x1C;          case 0x00: return 0x1D;          case ' ':  return 0x39;          
        case '1': return 0x02;
        case '2': return 0x03;
        case '3': return 0x04;
        case '4': return 0x05;
        case '5': return 0x06;
        case '6': return 0x07;
        case '7': return 0x08;
        case '8': return 0x09;
        case '9': return 0x0A;
        case '0': return 0x0B;
        case '-': return 0x0C;
        case '=': return 0x0D;
        case '[': return 0x1A;
        case ']': return 0x1B;
        case ';': return 0x27;
        case '\'': return 0x28;
        case '`': return 0x29;
        case '\\': return 0x2B;
        case ',': return 0x33;
        case '.': return 0x34;
        case '/': return 0x35;
        
        case '^': return 0x48;          case 'v': return 0x50;          case '<': return 0x4B;          case '>': return 0x4D;          
        case 'A': return 0x1E;
        case 'B': return 0x30;
        case 'C': return 0x2E;
        case 'D': return 0x20;
        case 'E': return 0x12;
        case 'F': return 0x21;
        case 'G': return 0x22;
        case 'H': return 0x23;
        case 'I': return 0x17;
        case 'J': return 0x24;
        case 'K': return 0x25;
        case 'L': return 0x26;
        case 'M': return 0x32;
        case 'N': return 0x31;
        case 'O': return 0x18;
        case 'P': return 0x19;
        case 'Q': return 0x10;
        case 'R': return 0x13;
        case 'S': return 0x1F;
        case 'T': return 0x14;
        case 'U': return 0x16;
        case 'V': return 0x2F;
        case 'W': return 0x11;
        case 'X': return 0x2D;
        case 'Y': return 0x2C;
        case 'Z': return 0x15;
        
        default: return -1;      }
}

static char scan_codes[256] = {
    0,       0,       '1',     '2',     '3',     '4',     '5',     '6',     '7',     '8',     '9',     '0',     '-',     '=',     '\b',     '\t',     'Q',     'W',     'E',     'R',     'T',     'Y',     'U',     'I',     'O',     'P',     '[',     ']',     '\n',     0,       'A',     'S',     'D',     'F',     'G',     'H',     'J',     'K',     'L',     ';',     '\'',     '`',     0,       '\\',     'Z',     'X',     'C',     'V',     'B',     'N',     'M',     0,       0,       0,       0,       0,       0,       ' ',     0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       '7',     '^',     '9',     '-',     '4',     '5',     '6',     '+',     '1',     'v',     '3',     '0',     '.',     0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0    };


void setPlayer1Up(uint64_t key){
    player1Up = key;
    player1Upbeak = key + 0x80;
}

void setPlayer1Down(uint64_t key){
    player1Down = key;
    player1Downbeak = key + 0x80;
}


void setPlayer2Up(uint64_t key){
    player2Up = key;
    player2Upbeak = key + 0x80;
}

void setPlayer2Down(uint64_t key){
    player2Down = key;
    player2Downbeak = key + 0x80;
}




void draw_paddle(Paddle* paddle, uint64_t color){
    call_draw_rectangle(color, paddle->x, paddle->y, paddle->width, paddle->height);
}


void clear_paddle(Paddle* paddle){
    draw_paddle(paddle, BACKGROUND_COLOR);
}


void clearBall(){
    call_draw_ball(BACKGROUND_COLOR, ball.size, ball.x, ball.y);
}

void movePaddle(Paddle* paddle){
    clear_paddle(paddle);     int move;

    if (paddle->direction == UP){
        move = paddle->y - movement_vector[(int)paddle->direction] * paddle->speed;         if (move >= BORDER_SIZE){             paddle->y = move;         } else {
            paddle->y = BORDER_SIZE;             paddle->direction = STOP;         }
    } else if (paddle->direction == DOWN){
        move = paddle->y + paddle->speed;         if (move + paddle->height <= SCREEN_HEIGHT - BORDER_SIZE){             paddle->y = move;         } else {
            paddle->y = SCREEN_HEIGHT - BORDER_SIZE - paddle->height;             paddle->direction = STOP;         }
    }

    draw_paddle(paddle, paddle->color); }





void showScoreCard(uint64_t hexColor){
    call_set_font_size(SCORE_CARD_FONT_SIZE);
    call_character_at(hexColor, player1Score+'0', SCORE_CARD_X, SCORE_CARD_Y);
    call_character_at(hexColor, player2Score+'0', SCORE_CARD_X+200, SCORE_CARD_Y);
}
void updateScore(int player){
    if (player == 1){
        call_character_at(BACKGROUND_COLOR, player1Score+'0', SCORE_CARD_X, SCORE_CARD_Y);
        player1Score++;
        call_character_at(WHITE, player1Score+'0', SCORE_CARD_X, SCORE_CARD_Y);
    } else {
        call_character_at(BACKGROUND_COLOR, player2Score+'0', SCORE_CARD_X+200, SCORE_CARD_Y);
        player2Score++;
        call_character_at(WHITE, player2Score+'0', SCORE_CARD_X+200, SCORE_CARD_Y);
    }
}
void clearScoreCard(){
    showScoreCard(BACKGROUND_COLOR);
}

char checkScored(){
    if (ball.x + ball.size / 2 < paddle1.x + paddle1.width){
        updateScore(2);
        return 1;
    }
    if (ball.x + ball.size / 2 > paddle2.x){
        updateScore(1);
        return 1;
    }
    return 0;
}       

void move_ball(){
    clearBall(); 
    if (checkScored()){         if (gsound){
            goal();         }
        clearBall();         resetGame();         return;
    }

    ball.x += ball.speedX;     ball.y += ball.speedY; 
    if (ball.x <= BORDER_SIZE){         ball.x = BORDER_SIZE;         ball.speedX *= -1;         if (sound == 1){
            bounce();         }
    } else if (ball.x + ball.size >= SCREEN_WIDTH - BORDER_SIZE){         ball.x = SCREEN_WIDTH - BORDER_SIZE - ball.size;         ball.speedX *= -1;         if (sound == 1){
            bounce();         }
    }

    if (ball.y <= BORDER_SIZE){         if (sound == 1){
            bounce();         }
        ball.y = BORDER_SIZE;         ball.speedY = -ball.speedY;     } else if (ball.y + ball.size >= SCREEN_HEIGHT - BORDER_SIZE * 2){         if (sound == 1){
            bounce();         }
        ball.y = SCREEN_HEIGHT - BORDER_SIZE * 2 - ball.size;         ball.speedY = -ball.speedY;     }

    if (ball.x <= paddle1.x + paddle1.width && ball.x + ball.size >= paddle1.x && ball.y + ball.size >= paddle1.y && ball.y <= paddle1.y + paddle1.height){
        if (sound == 1){
            bounce();         }
        if (paddle1.direction == STOP){
            ball.speedY = 0;
            ball.speedX = BALL_SPEED_HORIZONTAL;
        } else {
            if (paddle1.direction == UP)
                ball.speedY = -BALL_SPEED;
            if (paddle1.direction == DOWN)
                ball.speedY = BALL_SPEED;
            ball.speedX = BALL_SPEED;
        }
    }

    if (ball.x + ball.size >= paddle2.x && ball.x <= paddle2.x + paddle2.width && ball.y + ball.size >= paddle2.y && ball.y <= paddle2.y + paddle2.height){
        ball.speedX *= -1;
        if (sound == 1){
            bounce();         }
        if (paddle2.direction == STOP){
            ball.speedY = 0;
            ball.speedX = -1 * BALL_SPEED_HORIZONTAL;
        } else {
            if (paddle2.direction == UP)
                ball.speedY = -BALL_SPEED;
            if (paddle2.direction == DOWN)
                ball.speedY = BALL_SPEED;
            ball.speedX = BALL_SPEED * -1;
        }
    }

    call_draw_ball(ball.color, ball.size, ball.x, ball.y); }




void resetGame(){
    clear_paddle(&paddle1);
    clear_paddle(&paddle2);

    ball.x = SCREEN_WIDTH / 2 - ball.size / 2; 
    ball.y = SCREEN_HEIGHT / 2 - ball.size / 2;

    ball.speedX *= -1;
    ball.speedY = 0;

    paddle1.y = SCREEN_HEIGHT / 2 - paddle1.height / 2;
    paddle2.y = SCREEN_HEIGHT / 2 - paddle2.height / 2;

    draw_paddle(&paddle1, paddle1.color);
    draw_paddle(&paddle2, paddle2.color);
    draw_borders();
}

void draw_middle_line(){
    for (int i = 0; i < SCREEN_HEIGHT; i+= 20){
        call_draw_rectangle(BORDER_COLOR, SCREEN_WIDTH/2-5, i, 10, 10);
    }
}



void draw_borders(){
    call_draw_rectangle(BORDER_COLOR, 0, 0, SCREEN_WIDTH, BORDER_SIZE); 
    call_draw_rectangle(BORDER_COLOR, 0, SCREEN_HEIGHT - BORDER_SIZE, SCREEN_WIDTH, BORDER_SIZE);  
}
void pauseModular(uint64_t hexColor){
    call_set_font_size(10);
    call_draw_word_colorAt(hexColor, "PAUSED", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2-200);
    call_set_font_size(DEFAULT_FONT_SIZE);
    call_draw_word_colorAt(hexColor, "Press X to exit. ", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2+100);
    call_draw_word_colorAt(hexColor, "Press C to access configuration. ", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2+130);
    call_draw_word_colorAt(hexColor, "Press S to toggle bounce sound.", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2+160);
    call_draw_word_colorAt(hexColor, "Press G to toggle goal sound.", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2+190);
    call_draw_word_colorAt(hexColor, "Press R to reset.", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2+220);
    call_draw_word_colorAt(hexColor, "Press any other key to continue.", SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2+250);

}

void pauseGame(){
    pauseModular(WHITE);
}
void unpause(){
    pauseModular(BACKGROUND_COLOR);
}

void training(Paddle* paddle2){
    if (ball.y + ball.size < paddle2->y + paddle2->height / 2){
        paddle2->direction = UP;
    } else if (ball.y > paddle2->y + paddle2->height / 2){
        paddle2->direction = DOWN;
    } else {
        paddle2->direction = STOP;
    }

    movePaddle(paddle2);
}

int benchmark(){
    int ticks = call_ticks_elapsed();
    for (long i = 0; i < BENCHMARK_UPPER_BOUND; i++);
    int ticks2 = call_ticks_elapsed();
    return ticks2-ticks;
}

void sleepbm(int bm){
    for (long i = 0; i < SLEEP_BENCHMARK_UPPER_BOUND;i+=bm);
}
void options(){
    print("\tCONFIGURATION\n");
    print("Press 1 to change the ball speed\n");
    print("Press 2 to change the paddle speed\n");
    print("Press 3 to disable the experimental refresh rate\n");
    print("Press 4 to change the ball size\n");
    print("Press 5 to change border and middle line color\n");
    print("Press 6 to change the paddle height\n");
    print("Press 7 to change the paddle width\n");
    print("Press 8 to change the ball color\n");
    print("Press 9 to change the paddle color\n");
    print("Press 0 to change the background color\n");
    print("Press C to change the keybinds of P1\n");
    print("Press K to change the keybinds of P2\n");
    print("Press S to change the score to win.\n");
    print("Press D to restore deafult values\n");
    print("Press X to exit\n");
}


int getNumber(){
    char c = ' ';
    int number = 0;
    while (c != '\n'){
        c = get_c();
        if ((c >= '0' && c <= '9') || c == '\b'){
            if (c == '\b' && number != 0){
                number = number / 10;
                print("\b \b");
            } else {
                number = number * 10 + (c - '0');
                print("%c", c);
            }
        }
    }
    return number;
}


#define NUM_COMMON_COLORS 10

uint64_t COMMON_COLORS[NUM_COMMON_COLORS] = {
    0xFFFFFF,      0x000000,      0xFF0000,      0x00FF00,      0x0000FF,      0xFFFF00,      0xFF00FF,      0x00FFFF,      0x808080,      0xFFA500   };


void showColorOptions(){
    print("Choose a color:\n");
    for (int i = 0; i < NUM_COMMON_COLORS; i++){
        uint64_t color = COMMON_COLORS[i];
        print("%d", i+1);

        int firstSquareX = 50;
        int firstSquareY = (16 * DEFAULT_FONT_SIZE * i) + 70;
        int firstSquareSize = 25;

        int secondSquareSize = 20;
        int secondSquareX = firstSquareX + (firstSquareSize - secondSquareSize) / 2;
        int secondSquareY = firstSquareY + (firstSquareSize - secondSquareSize) / 2;

        call_put_square(firstSquareX, firstSquareY, firstSquareSize, WHITE);
        call_put_square(secondSquareX, secondSquareY, secondSquareSize, color);

        print("\n");
    }
    print("Enter the new color (1-10):");
}




void configuration(){
    char c;
   while (1){
        options();
        c = get_c();
        switch (c){
            case '1': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current ball speed: %d\n", BALL_SPEED);
                print("Enter the new ball speed: ");
                int speed = getNumber();
                if (speed > 0){
                    BALL_SPEED_HORIZONTAL = speed*2;
                    BALL_SPEED = speed;
                    ball.speedX = BALL_SPEED;
                }
                break;
            }
            case '2': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current paddle speed: %d \n", paddle1.speed);
                print("Enter the new paddle speed: ");
                int speed = getNumber();
                if (speed > 0){
                    paddle1.speed = speed;
                    paddle2.speed = speed;
                }
                break;
            }
            case '3': {
                experimental=0;
                break;
            }
            case '4': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current ball size: %d \n", ball.size);
                print("Enter the new ball size: ");
                int size = getNumber();
                if (size > 0){
                    ball.size=size;
                }
                break;
            }
            case '5': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current color:");
                call_put_square(SCREEN_WIDTH / 2 - 150, 2, 25, WHITE);
                call_put_square(SCREEN_WIDTH / 2 - 150 + (25 - 20) / 2, 2 + (25 - 20) / 2, 20, BORDER_COLOR);
                print("\n");
                showColorOptions();
                int color = getNumber(); 
                if (color >= 1 && color <= 10){
                    BORDER_COLOR = COMMON_COLORS[color-1];
                }
                break;
                
            }
            case '6': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current player1 paddle height: %d \n", paddle1.height);
                print("Enter the new paddle height: ");
                int height = getNumber();
                if (height > 0){
                    paddle1.height = height;

                }
                print("\nCurrent player2 paddle height: %d \n", paddle2.height);
                print("Enter the new paddle height: ");
                height = getNumber();
                if (height > 0){
                    paddle2.height = height;

                }
                break;
            }
            case '7': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current player1 paddle width: %d \n", paddle1.width);
                print("Enter the new paddle width: ");
            
                int width = getNumber();
                if (width > 0){
                    paddle1.width = width;
                }
                print("\nCurrent player2 paddle width: %d \n", paddle2.width);
                print("Enter the new paddle width: ");
              
                width = getNumber();
                if (width > 0){
                    paddle1.width = width;
                }
                break;
            }
            case '8': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current ball color: ");
                call_put_square(SCREEN_WIDTH / 2 - 80, 2, 25, WHITE);
                call_put_square(SCREEN_WIDTH / 2 - 80 + (25 - 20) / 2, 2 + (25 - 20) / 2, 20, ball.color);
                print("\n");
                showColorOptions();
                int color = getNumber(); 
                if (color >= 1 && color <= 10){
                    ball.color = COMMON_COLORS[color-1];
                }
                break;
            }
            case '9': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current paddle color: ");
                call_put_square(SCREEN_WIDTH / 2 - 50, 2, 25, WHITE);
                call_put_square(SCREEN_WIDTH / 2 - 50 + (25 - 20) / 2, 2 + (25 - 20) / 2, 20, PADDLE_COLOR);
                print("\n");
                showColorOptions();
                int color = getNumber();
                if (color >= 1 && color <= 10){
                        paddle1.color = COMMON_COLORS[color-1];
                        paddle2.color = COMMON_COLORS[color-1];
                }
                break;
            }
            case '0': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current background color: ");
                call_put_square(SCREEN_WIDTH / 2, 2, 25, WHITE);
                call_put_square(SCREEN_WIDTH / 2  + (25 - 20) / 2, 2 + (25 - 20) / 2, 20, BACKGROUND_COLOR);
                print("\n");
                showColorOptions();
                int color = getNumber();
                if (color >= 1 && color <= 10){
                    BACKGROUND_COLOR = COMMON_COLORS[color-1];
                }
                break;
            }
            case 'X': {
                call_clear_color(BACKGROUND_COLOR);
                return;
            }
            case 'C': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current up key: %c", scan_codes[player1Up]);
                print("\nEnter the new up key: ");

                char c;
 
                c = get_c();
                if (c != 0){
                    setPlayer1Up(asciiToKeyCode(c));
                }
                put_c(c);
                print("\nCurrent down key: %c", scan_codes[player1Down]);
                print("\nEnter the new down key: ");
                c = get_c();
                if (c != 0){
                    setPlayer1Down(asciiToKeyCode(c));
                }
                put_c(c);

                break;
            }
            case 'K': {
                call_clear_color(BACKGROUND_COLOR);
                print("Current up key: %c", scan_codes[player2Up]);
                print("\nEnter the new up key: ");
                char c;
                c = get_c();
                if (c != 0){
                    setPlayer2Up(asciiToKeyCode(c));
                }
                put_c(c);
                print("\nCurrent down key: %c", scan_codes[player2Down]);
                print("\nEnter the new down key: ");
                c = get_c();
                if (c != 0){
                    setPlayer2Down(asciiToKeyCode(c));
                }
                put_c(c);

                break;
            }
            case 'S':{
                call_clear_color(BACKGROUND_COLOR);
                print("Current score to win: %d \n", score);
                print("Enter the new score to win: ");
                int newScore = getNumber();
                if (newScore > 0){
                    score = newScore;
                }

            }
            case 'D':{
                BACKGROUND_COLOR = BLACK;
                set_paddle( &paddle1, 50, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_SPEED, STOP, WHITE);
                set_paddle( &paddle2, SCREEN_WIDTH - 50 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_SPEED, STOP, WHITE);
                set_ball(SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SPEED, BALL_SPEED, WHITE);
                setPlayer1Up(0x11);
                setPlayer1Down(0x1F);
                setPlayer2Up(0x48);
                setPlayer2Down(0x50);
                break;
            }
            default: {
                break;
            }
        }
        call_clear_color(BACKGROUND_COLOR);
   }
}



void set_paddle(Paddle *paddle, int x, int y, int width, int height, int speed, char directon, uint64_t color){
    paddle->x = x;
    paddle->y = y;
    paddle->width = width;
    paddle->height = height;
    paddle->speed = speed;
    paddle->direction = directon;
    paddle->color = color;

}
Ball ball;
void set_ball(int x, int y, int size, int speedX, int speedY, uint64_t color){
    ball.x = x;
    ball.y = y;
    ball.size = size;
    ball.speedX = speedX;
    ball.speedY = speedY;
    ball.color = color;
}


void pong(){
    call_clear_color(BACKGROUND_COLOR);
    set_paddle( &paddle1, 50, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_SPEED, STOP, WHITE);
    set_paddle( &paddle2, SCREEN_WIDTH - 50 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_SPEED, STOP, WHITE);
    set_ball(SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SPEED, BALL_SPEED, WHITE);
    setPlayer1Up(0x11);
    setPlayer1Down(0x1F);
    setPlayer2Up(0x48);
    setPlayer2Down(0x50);
    
    int setting = 0;
   
    call_set_font_size(10);
    call_draw_word_colorAt(WHITE, "PONG", SCREEN_WIDTH/2-270, SCREEN_HEIGHT/2-200);
    call_set_font_size(DEFAULT_FONT_SIZE);
    call_draw_word_colorAt(WHITE, "Press E to enable experimental refresh rate", 0, SCREEN_HEIGHT/2+100);
    call_draw_word_colorAt(WHITE, "Press any other key for default refresh rate", 0, SCREEN_HEIGHT/2+130);
    char e;
    e = get_c();
    if ( e == 'E'){
        call_draw_word_colorAt(BACKGROUND_COLOR, "Press E to enable experimental refresh rate", 0, SCREEN_HEIGHT/2+100);
        call_draw_word_colorAt(BACKGROUND_COLOR, "Press any other key for default refresh rate", 0, SCREEN_HEIGHT/2+130);
        call_draw_word_colorAt(WHITE, "Wait until benchmark is complete...", 0, SCREEN_HEIGHT/2+100);
        experimental = 1;
        setting = benchmark();
        call_draw_word_colorAt(BACKGROUND_COLOR, "Wait until benchmark is complete...", 0, SCREEN_HEIGHT/2+100);
        call_draw_word_colorAt(WHITE, "Benchmark complete!", 0, SCREEN_HEIGHT/2+100);
    }
    call_clear_color(BACKGROUND_COLOR);
     call_set_font_size(10);
    call_draw_word_colorAt(WHITE, "PONG", SCREEN_WIDTH/2-270, SCREEN_HEIGHT/2-200);
     call_set_font_size(DEFAULT_FONT_SIZE);
    call_draw_word_colorAt(WHITE, "Press T for training", 0, SCREEN_HEIGHT/2+100);
    call_draw_word_colorAt(WHITE, "Press C for configuration", 0, SCREEN_HEIGHT/2+130);
    call_draw_word_colorAt(WHITE, "Press any other key to begin 2 player PONG: ", 0, SCREEN_HEIGHT/2+160);
    call_draw_word_colorAt(WHITE, "Player 1 goes up with \"W\" and down with \"S\"", 0, SCREEN_HEIGHT/2+190);
    call_draw_word_colorAt(WHITE, "Player 2 goes up with \"^\" and down with \"v\"", 0, SCREEN_HEIGHT/2+220);
    int Training = 0;
    char t;
    t = get_c();
    if( t == 'C'){
        call_clear_color(BACKGROUND_COLOR);
        configuration();
    }
    if ( t == 'T'){
        Training = 1;
    }
    uint16_t c = 0;
    char p = 0;
      
    call_clear_color(BACKGROUND_COLOR);
    draw_borders();
    draw_middle_line();  
    draw_paddle(&paddle1, paddle1.color);  
    draw_paddle(&paddle2, paddle2.color);
    showScoreCard(WHITE);
    int pos = call_get_buffer_position();

   
    resetGame();
    while (1){
        showScoreCard(WHITE);
        if (c==0x19){
            pauseGame();
            p = get_c();
            if (p == 'X'){
                call_clear_color(BACKGROUND_COLOR);
                player1Score = 0;
                player2Score = 0;
                return;
            }
            if (p == 'C'){
                call_clear_color(BACKGROUND_COLOR);
                configuration();
                call_clear_color(BACKGROUND_COLOR);
            };
            if(p == 'S'){
               sound=!sound;
            }
            if(p == 'G'){
                gsound = !gsound;
            }
            if(p == 'R'){
                player1Score = 0;
                player2Score = 0;
                resetGame();
                call_clear_color(BACKGROUND_COLOR);
            }
            

            
            draw_borders();
            draw_middle_line();
            draw_paddle(&paddle1, paddle1.color);
            draw_paddle(&paddle2, paddle2.color);
            showScoreCard(WHITE);

            unpause();
        }
        while (pos <= call_get_buffer_position()){
            c = call_get_char_at(pos++-1);
            if (c == player1Up){
                paddle1.direction = UP;
            }
            else if (c == player1Upbeak){
                if (paddle1.direction == UP)
                    paddle1.direction = STOP; 
            }  

            else if (c == player1Down){ 
                paddle1.direction = DOWN;
            }

            else if (c == player1Downbeak){ 
                if (paddle1.direction == DOWN)
                    paddle1.direction = STOP;
            }
            if (!Training){
                 if (c == player2Up){
                    paddle2.direction = UP;
                }
                else if (c == player2Upbeak){
                    if (paddle2.direction == UP)
                        paddle2.direction = STOP; 
                }   

                else if (c == player2Down)
                    paddle2.direction = DOWN;

                else if (c == player2Downbeak){
                    if (paddle2.direction == DOWN)
                        paddle2.direction = STOP;
                }
            }
            }
            if (Training){
                training(&paddle2);
            }
            movePaddle(&paddle1);
            movePaddle(&paddle2);
            move_ball();

            if (player1Score >= score || player2Score >= score){
                call_clear_color(BACKGROUND_COLOR);
                call_set_font_size(6);
                call_set_font_size(DEFAULT_FONT_SIZE);
                call_draw_word_colorAt(WHITE, "GAME OVER", SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-170);
                if(player1Score>player2Score){
                    call_draw_word_colorAt(WHITE, "PLAYER 1 WINS", SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-130);
                }
                else{
                    call_draw_word_colorAt(WHITE, "PLAYER 2 WINS", SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-130);
                }
                call_draw_word_colorAt(WHITE, "Press X to exit. ", SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-100);
                call_draw_word_colorAt(WHITE, "Press any other key to play again.", SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-75);

                p = get_c();
                if (p == 'X'){
                    call_clear_color(BACKGROUND_COLOR);
                    return;
                }
               call_clear_color(BACKGROUND_COLOR);
                player1Score = 0;
                player2Score = 0;
                resetGame();
                draw_middle_line();
            }
            draw_middle_line();
            if (experimental)
                sleepbm(setting);
            else call_sleepms(1);
    }
}

void * get_pong(){
    return &pong;
}