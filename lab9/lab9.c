//Luiz Rodrigo Lacé Rodrigues
//DRE:118049873

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int nconsumidores;
int nprodutores;
int buffer[BUFFER_SIZE];

sem_t produtores;
sem_t slotVazio;
sem_t bufferLotado;


//thread consumidora
void *consumidor(void * args){
    int id = *(int *) args;

    while(1){
        sem_wait(&bufferLotado);

        for(int i = 0; i<BUFFER_SIZE; i++){
            printf("log.consumidor(%d,%d,%d)\n", id, buffer[i],i);
            sem_post(&slotVazio);
        }
        printf("log.BufferVazio(%d)\n", id);

        sleep(1);
    }
}

//thread produtora
void * produtor(void * args){

    static int in = 0;

    int id = *(int *) args;

    while (1){
        //espera por slot vazio 
        sem_wait(&slotVazio);
        sem_wait(&produtores);

        buffer[in] = rand() % 10;

        printf("log.produtor(%d,%d,%d)\n",id,buffer[in],in);

        in = (in+1) % BUFFER_SIZE;

        if(in == 0){
            printf("log.bufferCheio(%d)\n",id);
            sleep(1);
            sem_post(&bufferLotado);
        }

        sem_post(&produtores);
    }
}

int main(int argc, char ** argv){
    srand(time(NULL));
    //tratando a falta de parametros
    if (argc < 3){
        printf("Digite %s: <nconsumidores> <nprodutores>.\n", argv[0]);
		return 1;
    }

    // passando os parametro para as variaveis globais
    nconsumidores = atoi(argv[1]);
    nprodutores = atoi(argv[2]);

    // caso q quantidade de parametros não seja sufuciente 
    if (nprodutores < 1 || nconsumidores <1){
        printf("É preciso de pelo menos 1 consumidor e 1  produtor para a aplicação funcionar");
        return 2;
    }

    //incializando semaforos
    sem_init(&produtores, 0,1);
    sem_init(&slotVazio,0,BUFFER_SIZE);
    sem_init(&bufferLotado,0,0);

    //inicializando as threads
    pthread_t threads[nconsumidores+nprodutores];
    int id[nconsumidores+nprodutores];


    printf("import verifica\nlog = verifica.Log()\n");

    for(int i= 0; i<nprodutores; i++){
        id[i] = i;
        if(pthread_create(&threads[i], NULL, produtor,(void*) &id[i])){
            printf("ERRO - PTHREAD_CREATE (produtor)");
            return 1;
        }
    }

    for(int i= nprodutores; i<(nprodutores+nconsumidores) ; i++){
        id[i]=i;
        if(pthread_create(&threads[i], NULL, consumidor, (void*) &id[i])){
            printf("ERRO - PTHREAD_CREATE(consumidor)");
            return 1;
        }
    }

    //retorno threads
    for(int i = 0; i<nconsumidores+nprodutores; i++){
        if (pthread_join(threads[i],NULL)){
            printf("ERRO PTHREAD_JOIN");
            return 1;
        }
    }

    
    //liberando os semafotos
    sem_destroy(&produtores);
    sem_destroy(&slotVazio);
    sem_destroy(&bufferLotado);

    return 0;
}