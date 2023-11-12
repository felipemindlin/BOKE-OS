#include<pipe.h>
#include<mysemaphore.h>

pipe pipe_array[MAX_PIPES];

int maxPipe = 1;
int first_eof = 1;

void init_pipes(){
    pipe_array[0].name = 0; 

    for (int i = 0; i < MAX_PIPES; i++)
    {
        pipe_array[i].name = -1;
    }
    
}

int create_pipe(int name){
    int found = 0;
    for(int i = 0; i < MAX_PIPES && found == 0; i++){
        if(pipe_array[i].name == -1){
            found = i;
        }

        // No puede haber dos pipes con el mismo nombre
        if (pipe_array[i].name == name)
        {
            return -1;
        }
    }

    pipe_array[found].name = name;
    pipe_array[found].read_sem_id = (char *) my_sem_open(0, "pipe_read");
    pipe_array[found].write_sem_id = (char *) my_sem_open(PIPE_BUFFER_SIZE, "pipe_write");
    pipe_array[found].write_pos = 0;
    pipe_array[found].read_pos = 0;
    pipe_array[found].pipe_buffer = malloc(PIPE_BUFFER_SIZE);
    pipe_array[found].eof = 0;
    pipe_array[found].left_to_read = 0;

    if (name >= maxPipe)
    {
        maxPipe = name +1;
    }
    
    return found;
}

int pipe_write(int id, const char message[], unsigned int size){
    
    if (pipe_array[id].eof && !first_eof)
    {
        return -1;
    }
    
    for (int i = 0; i < size; i++)
    {
        my_sem_wait(pipe_array[id].write_sem_id); // Bloquea si no hay espacio para escribir

        if (pipe_array[id].write_pos == PIPE_BUFFER_SIZE)
        {
            pipe_array[id].write_pos = 0;
        }
        
        if (first_eof && pipe_array[id].eof)
        {
            first_eof = 0;
            pipe_array[id].pipe_buffer[pipe_array[id].write_pos++] = _EOF_;
            pipe_array[id].left_to_read++;
            my_sem_post(pipe_array[id].read_sem_id);
            return -1;
        }
        
        pipe_array[id].pipe_buffer[pipe_array[id].write_pos++] = (char) message[i];
        pipe_array[id].left_to_read++;
        my_sem_post(pipe_array[id].read_sem_id);
    }

    return 1;
    
}

int pipe_read(int id, char * dest, unsigned int size){
    
    if (pipe_array[id].eof && pipe_array[id].left_to_read == 0)
    {
        dest[0]= _EOF_;
        return -1;
    }
    
    for (int i = 0; i < size; i++)
    {
        my_sem_wait(pipe_array[id].read_sem_id); // Bloquea si no hay nada para leer

        if (pipe_array[id].read_pos == PIPE_BUFFER_SIZE)
        {
            pipe_array[id].read_pos = 0;
        }
        
        dest[i] = pipe_array[id].pipe_buffer[pipe_array[id].read_pos++];
        
        if (dest[i] == _EOF_)
        {
            pipe_array[id].left_to_read--;
            my_sem_post(pipe_array[id].write_sem_id);
            return -1;
        }
        
        pipe_array[id].left_to_read--;
        my_sem_post(pipe_array[id].write_sem_id);
    }
    return 0;
    
}

void close_pipe(int id){
    pipe_array[id].name = -1;
    pipe_array[id].read_pos = 0;
    pipe_array[id].write_pos = 0;
    pipe_array[id].eof = 0;

    my_sem_close(pipe_array[id].read_sem_id);
    my_sem_close(pipe_array[id].write_sem_id);
    free(pipe_array[id].pipe_buffer);
}

int get_pipe_id(int name){
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipe_array[i].name == name){
            return i;
        }
    }
    return -1;
}

pipe get_pipe(int id){
    return pipe_array[id];
}

void send_eof(int id){
    //pipe_array[id].eof = 1;
    //pipe_array[id].left_to_read++;
    //my_sem_post(pipe_array[id].read_sem_id);//should we do this?
}

void send_eof_to_foreground(){
    pcb_t * foreground_pcb = get_pcb_entry(get_process_foreground_pid());
    if(foreground_pcb == NULL){
        return;
    } else if(foreground_pcb->process->pid == 2){
        return;
    }
    pipe_array[foreground_pcb->process->fr].eof = 1;
}


int create_pipe_anonymous(){
    return create_pipe(maxPipe);
}

int eof_sent(int id){
    return pipe_array[id].eof;
}
