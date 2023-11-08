#include <idle.h>

void idle(){
    while(1){
        _hlt();
    }
}