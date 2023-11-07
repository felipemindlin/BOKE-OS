#include <idle.h>

void idle(){
    while(1){
        drawWord(".");
        _hlt();
    }
}