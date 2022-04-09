#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS 2 //total de threads a serem criadas

#define NUMBERS 10000

int elementos[NUMBERS];

//funcao executada pelas threads para elevar os numeros do vetor ao quadrado
void *ContaElemento (void *arg) {
  int numeroDaThread = *(int*) arg;
  int contador = 0;

  for(int i = 0+((NUMBERS/NTHREADS)*numeroDaThread); i<((NUMBERS/NTHREADS)*(numeroDaThread+1));i++){
      elementos[i] = elementos[i]*elementos[i];
      contador +=1;
  };

  printf("\n");
  printf("Fim da thread %d, foram alterados %d numeros no vetor\n",numeroDaThread,contador);
  pthread_exit(NULL);
}

//funcao principal do programa
int main() {
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int thread[NTHREADS];

  //completa o vetor com numeros 2
  for(int i = 0; i<NUMBERS; i++){
    elementos[i] = 2;
  }

  //cria as threads
  for(int i=0; i<NTHREADS; i++) {
    thread[i] = i;
    if (pthread_create(&tid_sistema[i], NULL, ContaElemento, (void*)&thread[i])){
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //--espera todas as threads terminarem
  for (int i=0; i<NTHREADS; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }

  //faz o teste dos numeros dentro do vetor
  for (int i = 0; i<NUMBERS; i++){
    if(elementos[i] != 4){
      printf("Erro\n");
      break;
    }
  }

  printf("\n");
  printf("--Thread principal terminou--\n");

  return 0;
}
