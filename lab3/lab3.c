//Luiz Rodrigo Lacé Rodrigues - DRE:118049873

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
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

    if(id == nthreads-1){
        fim = dim;
    }
    else{
        fim = ini + tamBloco; //trata o resto se houver
    }

    numerosParaRetorno *numeros;

    numeros = (numerosParaRetorno *) malloc (sizeof(numerosParaRetorno));
    if(numeros == NULL){
        fprintf(stderr,"ERRO -- malloc\n");
        exit(2);
    }

    numeros->num_maior = vetor[ini];
    numeros->num_menor = vetor[ini];
 
    //compara os elementos do bloco da thread
    for (long int i= ini; i<fim;i++){
        if(vetor[i]>numeros->num_maior){
            numeros->num_maior = vetor[i];
        }
        if(vetor[i]<numeros->num_menor){
            numeros->num_menor = vetor[i];
        }
    }

    //printf(" maior: %lf\n", numeros->num_maior);
    //printf(" menor: %lf\n", numeros->num_menor);


    // retorna o resultado da soma local
    pthread_exit((void*) numeros);
}

// fluxo principal
int main(int argc, char *argv[]){
    double ini, fim; // variaveis para tomada de tempo
    pthread_t *tid; // identificadores das threads no sistema
    numerosParaRetorno numerosFinais,*retorno; // valor de resultado final e retorno das threads

    // recebe e valida os parametros de entrada(dimensao do vetor, numero de threads)
    if(argc < 3){
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    // aloca o vetor de entrada
    vetor = (float *) malloc (sizeof(float)*dim);
    if(vetor == NULL){
        fprintf(stderr,"ERRO - malloc");
        return 2;
    }

    // preenche o vetor com numeros aleatorios
    srand(time(NULL));
    for (long int i=0; i<dim; i++){
        vetor[i] = rand()%10000000000000;
    }

    GET_TIME(ini);
    //busca sequencial dos elementos
    numerosParaRetorno sequencial;
    sequencial.num_maior = vetor[0];
    sequencial.num_menor = vetor[0];
    for(int i=0; i<dim; i++){
        if(vetor[i]>sequencial.num_maior){
            sequencial.num_maior = vetor[i];
        }
        if(vetor[i]<sequencial.num_menor){
            sequencial.num_menor = vetor[i];
        }
    }
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n",fim-ini);



    // busca concorrente dos elementos
    GET_TIME(ini);
    //alocando estruturas
    tid = (pthread_t *) malloc (sizeof(pthread_t) * nthreads);
    if(tid == NULL){
        fprintf(stderr,"ERRO -- malloc\n");
        return 2;
    }
    retorno = (numerosParaRetorno *) malloc (sizeof(numerosParaRetorno) * nthreads);
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

        //printf("maior resultado: %lf\n", retorno->num_maior);
        //printf("menor resultado: %lf\n", retorno->num_menor);

        
        if(i==0){
            numerosFinais.num_maior = retorno->num_maior;
            numerosFinais.num_menor = retorno->num_menor;
        }

        if(retorno->num_maior > numerosFinais.num_maior){
            numerosFinais.num_maior = retorno->num_maior;
        }

        if(retorno->num_menor < numerosFinais.num_menor){
            numerosFinais.num_menor = retorno->num_menor;
        }
        
    }

    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n",fim-ini);
    
    // exibir os resultados
    //printf("Maior: %lf\n",numerosFinais.num_maior);
    //printf("Menor: %lf\n",numerosFinais.num_menor);


    // libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}