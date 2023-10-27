#include <idle.h>

void idle(){
    while(1){
        __asm__("hlt");
    }
}