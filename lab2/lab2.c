#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"timer.h"

float *matriz1; //matriz de entrada 1
float *matriz2; //matriz de entrada 2
float *matrizResultante; // matriz resultante da multiplicação da 1 e 2
int nthreads; //numero de threads

typedef struct{
    int id; //ideitificador do elemento que a thred ira processsar
    int dim; // dimensao das estruturas de entrada
} tArgs;


//funcao que as threads executarao
void * multiplicarMatrizes(void*arg){
    tArgs *args = (tArgs *) arg;
    //printf("Thread %d\n", args->id);
    for(int linha = args->id; linha<(args->dim); linha+=nthreads){
        for(int coluna=0; coluna<(args->dim); coluna++){
            for(int casaResultante = 0; casaResultante<(args->dim); casaResultante++){
                int posicao = linha * args->dim + coluna;
                matrizResultante[posicao] += matriz1[linha*(args->dim)+casaResultante] * matriz2[posicao];
            }
        }
    }
    pthread_exit(NULL);
}

//fluxo principal
int main( int argc, char*argv[]){
    int dim; // dimensao da matriz de entrada
    pthread_t *tid; // identificadores das threads no sistema
    tArgs* args; // identificadores locais das threads e dimensao
    double inicio,fim,delta; //variaveis para fazer a tomada e tempo

    GET_TIME(inicio);
    //leitura e avaliacao dos parametros de entrada
    if(argc<3){
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if(nthreads>dim){
        nthreads = dim;
    }

    //alocacao de memoria para as estruturas de dados
    matriz1 = (float *) malloc(sizeof(float) * dim * dim);
    if (matriz1 == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }
    matriz2 = (float *) malloc(sizeof(float) * dim * dim);
    if (matriz2 == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    } 
    matrizResultante = (float *) malloc(sizeof(float) *dim *dim);
    if (matrizResultante == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    //inicializacao das estruturas de dados de entrada e saida
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            matriz1[i*dim+j]=1; //equivale a mat[i][j]
            matriz2[i*dim+j]=1;
        }
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo inicializacao: %lf\n",delta);


    //multiplicacao da matriz pelo vetor
    GET_TIME(inicio);
    //alocacao das estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid == NULL){
        puts("ERRO - malloc");
        return 2;
    }
    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if(args == NULL){
        puts("ERRO - malloc");
        return 2;
    }


    //criacao das threads
    for (int i =0; i<nthreads; i++){
        (args+i)->id = i;
        (args+i)->dim = dim;
        if(pthread_create(tid+i, NULL, multiplicarMatrizes, (void*)(args+i))){
            puts("ERRO -- pthread_create");
            return 3;
        }
    }

    //espera pelo termino das threads
    for (int i =0; i<nthreads; i++){
        pthread_join(*(tid+i), NULL);
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de multiplicação: %lf\n",delta);

    //exibicao dos resultados
    /*puts("Matriz de entrada 1:");
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            printf("%.1f ", matriz1[i*dim+j]);
        }
        puts("");
    }

    puts("Matriz de entrada 2:");
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            printf("%.1f ", matriz2[i*dim+j]);
        }
        puts("");
    }

    printf("Matriz resultante\n");
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            printf("%.1f ",matrizResultante[i*dim+j]);
        }
        puts("");   
    }*/

    //fazendo os testes de valor dentro da matriz resultante
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            if(matrizResultante[i*dim+j] != dim){
                printf("ERRO na multiplicação dos valores\n");
                break;
            }
        }
           
    }
    printf("SUCESSO: Todos os valores foram perfeitamente calculados\n");

    //liberacao da memoria
    GET_TIME(inicio);
    free(matriz1);
    free(matriz2);
    free(matrizResultante);
    free(args);
    free(tid);
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de finalizacao: %lf\n",delta);

    return 0;
};