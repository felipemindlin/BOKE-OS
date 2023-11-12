#include <lib.h>
#include <naiveConsole.h>
#include "include/keyboard_driver.h"
#include "include/videoDriver.h"
#include <keyboard_buffer.h>
#include "defs.h"
#include "interrupts.h"
#include "registers.h"
#include "include/ScanCodes.h"
#include "../include/pipe.h"
#include <mysemaphore.h>
#include "include/syscalls.h"

char sem_id_keyboard[]="keyboard";

void keyboard_handler(){
    uint16_t key = get_key();
    static int shift_pressed = 0, ctrl_pressed = 0;
    
    if (key == 0x00)
        return;
        
    if (key == 0x2A || key == 0x36){
        shift_pressed = 1;
        return;
    }
    else if (key == 0xAA || key == 0xB6){
        shift_pressed = 0;
        return;
    }
    else if (key == 0x1D){
        ctrl_pressed = 1;
        return;
    }
    else if (key == 0x9D){
        ctrl_pressed = 0;
        return;
    }
    
    uint16_t * buff = get_buffer_address();
    int buff_pos = get_buffer_position();
    if(ctrl_pressed){
         if(scan_codes[key] == 'C'){
            kill_foreground_process();
            clear_buffer();
            if(get_process_started()){my_sem_post(sem_id_keyboard);}
            
            return;
        } else if(scan_codes[key] == 'D'){
            if(get_pcb_entry(get_process_foreground_pid())->process->fr == SHELL){
                key = _EOF_;
                shift_pressed = 0;
                
            } else {
                send_eof_to_foreground();
                return;
            }
            
            
        } else if(scan_codes[key] == 'P'){
            print_process();
            return;
        } else if(scan_codes[key] == 'L'){
            clear();
            return;
        return;
        } else {
            return;
        }
    }
    if(shift_pressed){
        if(scan_codes[key] == '7'){
            key=0x54;
        }
        else if(scan_codes[key] == '\\'){
            key=0x55;
        }
    }
   
    if (buff_pos+1 < BUFF_SIZE){
        set_pos(buff_pos+1); 
        buff[buff_pos+1] = 0;  
    } else {
        set_pos(0);  
        buff[0] = 0;  
    }
    
    buff[buff_pos] = key; 
    
    if (scan_codes[key] == ';'){
        saveState();
        flag_snapshot_taken = 1;
    }
    if(get_process_started()){my_sem_post(sem_id_keyboard);}
    return; 
}





