
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

char key_sem_id[]="key";

void sys_write(char *buf, int len, int filedescriptor){

    pcb_t * currentPCB = get_current_pcb();

    if (currentPCB->process->fw == SHELL && currentPCB->process->pid == get_process_foreground_pid())
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
        pipe_write(currentPCB->process->fw,buf,len);
    }
    
}

void sys_read( char *buf, int len, int filedescriptor){

    pcb_t * currentPCB = get_current_pcb();
    
    if (currentPCB->process->fr == SHELL)
    {
        
        switch (filedescriptor){
        case STDIN:
            my_sem_open(1,key_sem_id);
            my_sem_wait(key_sem_id);
            if(current_process_id()!=SHELL_PID){
                my_sem_close(key_sem_id);
            }
            int pos = getBufferPosition();
            char aux = 0;
            for (int i = 0; i < len; ){
                _hlt();
                aux = getCharAt(pos);
                if (aux > 0 && aux <= 255){
                    if (aux == 0x39)
                        buf[i++]=' ';
                    else if(aux==_EOF_)
                        buf[i++]=_EOF_;
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
        pipe_read(currentPCB->process->fr,buf,len);
    }
    
}

