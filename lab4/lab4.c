// Luiz Rodrigo Lacé Rodrigues
// DRE: 118049873
/*Atividade 4: Projetar e implementar um programa concorrente onde a ordem de execuc ̧ao ̃
das threads e controlada no programa.*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 4
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond1,x_cond2;


/* Thread A */
void *A (void *t) {
  int boba1, boba2;
  
  /* faz alguma coisa pra gastar tempo... */
  boba1=10000; boba2=-10000; while (boba2 < boba1) boba2++;
  
  pthread_mutex_lock(&x_mutex);
  if (x<3) {
      pthread_cond_wait(&x_cond2,&x_mutex);
  }

  printf("Volte sempre!\n");
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}


void *B (void *t) {

  pthread_mutex_lock(&x_mutex);
  if (x<1) {
      pthread_cond_wait(&x_cond1,&x_mutex);
  }
  x++;
  printf("Fique a vontade.\n");
  if(x==3){
      pthread_cond_signal(&x_cond2);
  }
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

void *C (void *t) {

  pthread_mutex_lock(&x_mutex);
  if (x<1) {
      pthread_cond_wait(&x_cond1,&x_mutex);
  }
  x++;
  printf("Sente-se por favor.\n");
  if(x==3){
      pthread_cond_signal(&x_cond2);
  }
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

void *D (void *t) {

  pthread_mutex_lock(&x_mutex);
  printf("Seja bem-vindo!\n");
  x++;
  pthread_cond_broadcast(&x_cond1);
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}



int main(void){
    int i;
    pthread_t threads[NTHREADS];

    //Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao
    pthread_mutex_init(&x_mutex,NULL);
    pthread_cond_init (&x_cond1, NULL);
    pthread_cond_init (&x_cond2, NULL);

    //criando as threads
    pthread_create(&threads[0],NULL,A,NULL);
    pthread_create(&threads[1],NULL,B,NULL);
    pthread_create(&threads[2],NULL,C,NULL);
    pthread_create(&threads[3],NULL,D,NULL);


    //Esperando as threads completarem
    for(int i = 0;i<NTHREADS;i++){
        pthread_join(threads[i],NULL);
    }

    //desalocando variaveis
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond1);
    pthread_cond_destroy(&x_cond2);

    return 0;
}