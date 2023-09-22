#include "include/UserSyscalls.h"
 
 void goal(){
    call_beep(261, 200); 
    call_sleepms(50);
    call_beep(294, 125);
    call_beep(392, 325);
 }
 
 void bounce(){
     call_beep(100, 1); 
 }
 