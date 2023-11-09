#include<pipe.h>
#include<mysemaphore.h>

pipe pipeArray[MAX_PIPES];

//int maxPipe = 1;
//int firstEOF = 1;

void init_pipes(){
    pipeArray[0].name = 0; 

    for (int i = 0; i < MAX_PIPES; i++)
    {
        pipeArray[i].name = -1;
    }
    
}

int create_pipe(int name){
    int found = 0;
    for(int i = 0; i < MAX_PIPES && found == 0; i++){
        if(pipeArray[i].name == -1){
            found = i;
        }

        // No puede haber dos pipes con el mismo nombre
        if (pipeArray[i].name == name)
        {
            return -1;
        }
    }

    pipeArray[found].name = name;
    pipeArray[found].readSemId = my_sem_open(0, "pipeRead");
    pipeArray[found].writeSemId = my_sem_open(PIPE_BUFFER_SIZE, "pipeWrite");
    pipeArray[found].writePos = 0;
    pipeArray[found].readPos = 0;
    pipeArray[found].pipeBuffer = malloc(PIPE_BUFFER_SIZE);
    //pipeArray[found].eof = 0;
    //pipeArray[found].leftToRead = 0;
/*
    if (name >= maxPipe)
    {
        maxPipe = name +1;
    }
    */
    return found;
}

int pipe_write(int id, const char message[], unsigned int size){
    /*
    if (pipeArray[id].eof && !firstEOF)
    {
        return -1;
    }
    */
    for (int i = 0; i < size; i++)
    {
        my_sem_wait(pipeArray[id].writeSemId); // Bloquea si no hay espacio para escribir

        if (pipeArray[id].writePos == PIPE_BUFFER_SIZE)
        {
            pipeArray[id].writePos = 0;
        }
        /*
        if (firstEOF && pipeArray[id].eof)
        {
            firstEOF = 0;
            pipeArray[id].pipeBuffer[pipeArray[id].writePos++] = EOF;
            //pipeArray[id].leftToRead++;
            my_sem_post(pipeArray[id].readSemId);
        }
        */
        pipeArray[id].pipeBuffer[pipeArray[id].writePos++] = (char) message[i];
        //pipeArray[id].leftToRead++;
        my_sem_post(pipeArray[id].readSemId);
    }

    return 1;
    
}

int pipe_read(int id, char * dest, unsigned int size){
    
    /*
    if (pipeArray[id].eof && pipeArray[id].leftToRead == 0)
    {
        dest[0]= EOF;
        return -1;
    }
    */
    for (int i = 0; i < size; i++)
    {
        my_sem_wait(pipeArray[id].readSemId); // Bloquea si no hay nada para leer

        if (pipeArray[id].readPos == PIPE_BUFFER_SIZE)
        {
            pipeArray[id].readPos = 0;
        }
        
        dest[i] = pipeArray[id].pipeBuffer[pipeArray[id].readPos++];
        /*
        if (dest[i] == -1)
        {
            pipeArray[id].leftToRead--;
            my_sem_post(pipeArray[id].writeSemId);
            return -1;
        }
        */
        //pipeArray[id].leftToRead--;
        my_sem_post(pipeArray[id].writeSemId);
    }
    return 0;
    
}

void close_pipe(int id){
    pipeArray[id].name = -1;
    pipeArray[id].readPos = 0;
    pipeArray[id].writePos = 0;
    //pipeArray[id].eof = 0;

    my_sem_close(pipeArray[id].readSemId);
    my_sem_close(pipeArray[id].writeSemId);
    free(pipeArray[id].pipeBuffer);
}

int getPipeID(int name){
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipeArray[i].name == name){
            return i;
        }
    }
    return -1;
}

pipe getPipe(int id){
    return pipeArray[id];
}

void send_eof(int id){
    //pipeArray[id].eof = 1;
    //pipeArray[id].leftToRead++;
    //my_sem_post(pipeArray[id].readSemId);//should we do this?
}

void send_eof_to_foreground(){

}
