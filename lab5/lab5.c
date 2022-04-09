//Luiz Rodrigo Lacé Rodrigues
//DRE: 118049873
// LAB 5

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int nthreads;
int *vetorGlobal;
int threadsBarradas=0;

pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//funcao auxiliar para barrar as threads
void barreira(int id){
    pthread_mutex_lock(&x_mutex);
    if (threadsBarradas == (nthreads - 1 )){
        printf("A thread %d está liberando as outras threads\n",id);
        pthread_cond_broadcast(&x_cond);
        threadsBarradas = 0;
    } else {
        threadsBarradas++;
        int threadsFaltantes = nthreads - threadsBarradas;
        printf("A thread %d está barrada esperando as outras %d faltantes \n",id,threadsFaltantes);
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);
}

//função que será executada pelas threads, todos os valores do vetor global são somados em uma variavel
//Depois do somatorio as threads colocam um valor aleatorio na sua posição equivalente no vetor
void* soma(void*args){
    srand(time(NULL));  //seed para os numeros aleatorios
    int id = *(int*) args;
    //alocando memoria para as somas locais
    int *somaLocal;
    somaLocal = (int  *) malloc (sizeof(int));
    if(somaLocal == NULL){
        fprintf(stderr,"ERRO - malloc");
        exit (1);
    }

    for(int i = 0; i<nthreads;i++){
        for(int j=0; j<nthreads; j++){
            *somaLocal += vetorGlobal[j];
        }
        printf("Fim do somatorio da Thread %d. A somaLocal atual é: %d \n", id, *somaLocal);
        barreira(id);
        //colocando valor aleatorio na posicao do vetor igual a o numero da thread
        vetorGlobal[id] = rand() %10;
        printf("O valor da posicao da thread %d no vetor global agora é %d\n", id, vetorGlobal[id]);
        barreira(id);
    }
    pthread_exit((void*)somaLocal);
}

int main(int argc, char * argv[]){
    srand(time(NULL)); // seed para os numeros aleatorios
    
    // validadando entrada
    if (argc < 2){
        printf("Digite %s <número de threads>\n", argv[0]);
        return 1;
    }

    //passando para a variavel global
    nthreads = atoi(argv[1]);

    //alocando memoria para o vetor global
    vetorGlobal = (int *) malloc (sizeof(int)*nthreads);
    if(vetorGlobal == NULL){
        fprintf(stderr,"ERRO - malloc");
        return 2;
    }

    int id[nthreads];
    long int resultado[nthreads];
    long int * resultadosParciais;
    pthread_t *tid;

    //alocando memoria outras estruturas
    resultadosParciais = (long int *) malloc (sizeof(long int));
    if(vetorGlobal == NULL){
        fprintf(stderr,"ERRO - malloc");
        return 2;
    }
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL){
        fprintf(stderr,"ERRO - malloc");
        return 2;
    }

    pthread_cond_init(&x_cond,NULL);
    pthread_mutex_init(&x_mutex, NULL);

    //colocando numeros aleatorios no vetor
    for(int i=0; i<nthreads;i++){
        vetorGlobal[i] = rand()%10;
    }

    //verificando se numeros aleatorios foram colocados no vetor
    // for(int i = 0;i<nthreads;i++){
    //     printf("%d ",vetorGlobal[i]);
    // }
    // printf("\n");

    //criando as threads
    for(int i=0; i<nthreads; i++){
        id[i] = i;
        pthread_create(tid+i,NULL, soma, (void*)&id[i]);
    }

    for(int i = 0; i<nthreads; i++){
        pthread_join(*(tid+i),(void **)&resultadosParciais);
        resultado[i] = *resultadosParciais;
    }

    //corretude 
    for(int i = 0; i<nthreads; i++){
        if(resultado[i] != resultado[0]){
            printf("Erro de corretude");
        }
    }
    printf("\n");
    printf("Todos os valores são iguais\n");

    //printando as somas finais
    for(int i = 0; i<nthreads;i++){
        printf("%ld ", resultado[i]);
    }
    printf("\n");

    free(vetorGlobal);
    free(resultadosParciais);
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);

    return 0;
}

