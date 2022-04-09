//Luiz Rodrigo Lacé Rodrigues - DRE:118049873

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include<time.h>
#include"timer.h"

long int dim; //dimensao do vetor de entrada
int nthreads; // numero de threads
float *vetor; //vetor de entrada com dimensao dim

typedef struct{
    float num_menor;
    float num_maior;
} numerosParaRetorno;

// fluxo de threads
void * tarefa(void * arg){
    long int id = (long int) arg; //identificador da threads    
    long int tamBloco = dim/nthreads; //tamanho do bloco de cada thread
    long int ini = id * tamBloco; // elemento inicial do bloco da thread
    long int fim = ini + tamBloco; // elemento final(não processado) do bloco da thread
    double * totalBloco;

    totalBloco = (double *) malloc (sizeof(double));
    if(totalBloco == NULL){
        fprintf(stderr,"ERRO -- malloc\n");
        exit(1);
    }

    if(id == nthreads-1){
        fim = dim;
    }
    else{
        fim = ini + tamBloco; //trata o resto se houver
    }

    for (int i = ini; i<fim; i++){
        totalBloco[0] += 4*(pow(-1,i)*(1/(double)((2*i)+1)));
    }

    pthread_exit((void*)totalBloco);
}

// fluxo principal
int main(int argc, char *argv[]){
    double ini, fim,total; // variaveis para tomada de tempo
    pthread_t *tid; // identificadores das threads no sistema
    double *retorno; // valor de resultado final e retorno das threads

    // recebe e valida os parametros de entrada(dimensao do vetor, numero de threads)
    if(argc < 3){
        fprintf(stderr, "Digite: %s <dimensao> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);



    // busca concorrente dos elementos
    GET_TIME(ini);
    //alocando estruturas
    tid = (pthread_t *) malloc (sizeof(pthread_t) * nthreads);
    if(tid == NULL){
        fprintf(stderr,"ERRO -- malloc\n");
        return 2;
    }
    retorno = (double *) malloc (sizeof(double));
    if(retorno == NULL){
        fprintf(stderr,"ERRO -- malloc\n");
        return 2;
    }

    // criar as threads
    for(long int i = 0; i<nthreads; i++){
        if(pthread_create(tid+i,NULL, tarefa,(void*) i)){
            fprintf(stderr,"ERRO -- pthread_create\n");
            return 3;
        }
    }

    // aguardar o termino das threads
    for(long int i = 0; i<nthreads; i++){
        if(pthread_join( *(tid+i),(void**) &retorno)){
            fprintf(stderr,"ERRO -- pthread_create\n");
            return 3;
        }

        total += retorno[0];
        
    }

    printf("total: %lf\n", total);

    GET_TIME(fim);
    //printf("Tempo concorrente: %lf\n",fim-ini);
    
    // exibir os resultados
    //printf("Maior: %lf\n",numerosFinais.num_maior);
    //printf("Menor: %lf\n",numerosFinais.num_menor);


    // libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}