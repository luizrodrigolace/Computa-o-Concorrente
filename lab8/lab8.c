//Luiz Rodrigo Lacé Rodrigues
//DRE:118049873

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4

//variaveis globais

int x = 0;      //variavel compartilhada entre as threads
sem_t condt1, condt2;     //semaforos para sincronizar a ordem de execucao das threads

void* t1(void* arg){
    sem_wait(&condt1);
    printf("Fique a vontade.\n");
    sem_post(&condt2);

    pthread_exit(NULL);
};
void* t2(void* arg){
    printf("Seja bem-vindo!\n");
    sem_post(&condt1);
    sem_post(&condt1);
    pthread_exit(NULL);
};
void* t3(void* arg){
    sem_wait(&condt2);
    sem_wait(&condt2);

    printf("Volte sempre!\n");

    pthread_exit(NULL);

};
void* t4(void* arg){
    sem_wait(&condt1);
    printf("Sente-se por favor\n");
    sem_post(&condt2);

    pthread_exit(NULL);

};

//funcao principal
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];

  //inicia os semaforos
  sem_init(&condt1, 0, 0);
  sem_init(&condt2, 0, 0);

  //cria as tres threads
  if (pthread_create(&tid[0], NULL, t1, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[1], NULL, t2, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[2], NULL, t3, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[3], NULL, t4, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }

  //--espera todas as threads terminarem
  for (int i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 

  return 0;  
}