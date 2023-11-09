
#include <naiveConsole.h>
#include "include/syscalls.h"
#include "drivers/include/videoDriver.h"
#include "include/defs.h"
#include "drivers/include/keyboard_driver.h"
#include "keyboard_buffer.h"
#include "include/interrupts.h"
#include "drivers/include/ScanCodes.h"
#include "./include/scheduler.h"
#include "./include/process.h"
#include "./include/pipe.h"



void sys_write(char *buf, int len, int filedescriptor){

    pcb_t * currentPCB = get_current_pcb();

    if (currentPCB->process->fw == SHELL)
    {
        switch (filedescriptor){
        case STDOUT: drawWordLen(buf, len);
            return;
        case STDERR: drawWordColorLen(RED, buf, len);
            return;
        default: invalidFd();
        }
    }
    else{
        int w = pipe_write(currentPCB->process->fw,buf,len);
    }
    
}

void sys_read( char *buf, int len, int filedescriptor){

    pcb_t * currentPCB = get_current_pcb();
    
    if (currentPCB->process->fr == SHELL)
    {
        switch (filedescriptor){
        case STDIN:;
            int pos = getBufferPosition();
            char aux = 0;
            for (int i = 0; i < len; ){
                _hlt();
                aux = getCharAt(pos);
                if (aux > 0 && aux <= 255){
                    if (aux == 0x39)
                        buf[i++]=' ';
                    else
                        buf[i++]=ScanCodes[(int)aux];
                    setPos(pos+1);
                }
                pos = getBufferPosition();
            }
            return;
        default: invalidFd();
    }
    }
    else{
        int r = pipe_read(currentPCB->process->fr,buf,len);
    }
    
}

