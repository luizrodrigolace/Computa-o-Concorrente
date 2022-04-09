#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"timer.h"

long int ntabuadas;
long int nmultiplos;
int nthreads;
long int *vetorGlobal;

void * tarefa (void* arg){
    long int id = (long int) arg; //identificador da threads

    long int quantTabuadas = ntabuadas/nthreads; //tamanho da quantidade de tabuadas de cada thread
    long int ini = (id * quantTabuadas)+1; // elemento tabuada da thread
    long int fim = ini + quantTabuadas; // tabuada final(não processada) da quantidade de tabuadas da thread

    //tratando a quantidade faltante de tabuadas
    if(id == nthreads-1){ fim = ntabuadas;}
    else{fim = (ini-1) + quantTabuadas;} //trata o resto se houver
    
    //iniciliazando vetor e variavel
    
    long int pos = id * (quantTabuadas)*nmultiplos;
    
    //fazendo a multiplicação
    for(int i=ini; i<=fim; i++){
        for(int j=1; j<=nmultiplos;j++){
            vetorGlobal[pos] = i*j;
            pos++;
        }
    }
}


int main(int argc, char *argv[]){
    pthread_t *tid; // identificadores de threads no sistema
    double ini, fim; //variaveis para tomada de tempo

    // validar o numero de entradas 
    if(argc < 4){
        fprintf(stderr, "Digite: %s <ntabuadas> <nmult> <nthreads>\n", argv[0]);
        return 1;
    }

    // Passando os valores para variaveis globais
    ntabuadas = atoi(argv[1]);
    nmultiplos = atoi(argv[2]);
    nthreads = atoi(argv[3]);

    //alocando memoria para o vetor
    vetorGlobal = (long int *) malloc (sizeof(long int)*(ntabuadas*nmultiplos));
    if(vetorGlobal == NULL){
        fprintf(stderr,"ERRO - malloc");
        return 2;
    }

    int long vetorSequencial[ntabuadas*nmultiplos];
    int posSeq = 0;

    //fazendo calculo sequencial
    GET_TIME(ini);
    for (int i = 1; i<=ntabuadas; i++ ){
        for(int j = 1; j<=nmultiplos; j++){
            vetorSequencial[posSeq] = i*j;
            posSeq ++;
            //printf("%d x %d = %ld\n",i,j,resultado);
        }
        //printf("\n");
    }
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim-ini);


    //alocando estruturas
    tid = (pthread_t *) malloc (sizeof(pthread_t) * nthreads);
    if(tid == NULL){fprintf(stderr,"ERRO -- malloc\n");return 2;}

    GET_TIME(ini)
    //criando as threads
    for(long int i = 0; i<nthreads; i++){
        if(pthread_create(tid+i,NULL, tarefa,(void*) i)){
            fprintf(stderr,"ERRO -- pthread_create\n");
            return 3;
        }
    }

    int posicaoGlobal = 0;
    //retornando as threads
    for(long int i = 0; i<nthreads; i++){
        if(pthread_join( *(tid+i),NULL)){
            fprintf(stderr,"ERRO -- pthread_create\n");
            return 3;
        }
    }
    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim-ini);

    //Verificando a corretude
    for(int i = 0; i<ntabuadas*nmultiplos;i++){
        if(vetorGlobal[i] != vetorSequencial[i]){
            printf("ERRO DE CORRETUDE\n");
        }
    }

    // Inicializando as matrizes
    long int matrix[ntabuadas][nmultiplos];
    for (int x = 0;x<ntabuadas;x++){
        for(int y = 0; y<nmultiplos;y++){
            matrix[x][y] = 0;
        }
    }

    // Passando os valores para a matriz
    int posicao = 0;
    for(int x = 0; x<ntabuadas;x++){
        for(int y= 0; y<nmultiplos;y++){
            matrix[x][y] = vetorGlobal[posicao];
            posicao++;
            printf("%ld\t",matrix[x][y]);
        }
        printf("\n");
    }

    
    free(tid);
    return 0;
}
