
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

int process_started = 0;

void set_process_started(int value){
    process_started = value;
}

int get_process_started(){
    return process_started;
}

char key_sem_id[]="keyboard";

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
    int current_pid = current_process_id();
    if(get_process_foreground_pid() != current_pid){
        add_to_queue(0, current_pid);
        os_block_current_process();
        finish_current_tick();
        return;
    }

    if (currentPCB->process->fr == SHELL)
    {
        switch (filedescriptor){
        case STDIN:
            int pos = getBufferPosition();
            char aux = 0;
            for (int i = 0; i < len; ){
                //_hlt(); I would not use hlt bc we already have sem_wait
                if(get_process_started()){my_sem_wait(key_sem_id);}
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

