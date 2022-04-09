#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<stdbool.h>
#include <unistd.h>
#include"timer.h" //macro para a tomada de tempo

//variaveis para sincronização
pthread_mutex_t mutex,mutex_escrita; //variavel de exclusao mutua, mutex_escrita serve para fazer a inserção no arquivo de forma concorrente
pthread_cond_t cond_cons, cond_prod; //variaveis de condição

#define P 1 //qtd de threads produtoras
#define TAM_BUFFER 10 //tamanho do bloco, guarda 10 blocos de tamanho N

FILE * arqEntrada; //arquivo entrada
FILE * arqSaida;   //arquivo saida

long int N; // tamanho do bloco
int C;      // numero de threads
int quantValores, quantValoresTotal, quantValoresEscritos=0; //variaveis para a verificação de escrita
int *buffer; //buffer onde os slots serão guardados

int slotsDisponiveis = 0;
int in = 0;  //index de entrada
int out = 0; //index de saida

void insere(){
    pthread_mutex_lock(&mutex); //exclusão mutua para acessar a variavel slotsDisponiveis

    while(slotsDisponiveis == 10){
        pthread_cond_wait(&cond_prod, &mutex); //Caso o buffer esteja cheio, espera
    }
    
    for(int j=0; j<N;j++){
        fscanf(arqEntrada, "%d" , &buffer[in*N+j]);
        //buffer[in*N+j] = atoi(buff);
    }

    
    quantValores -= N;          //diminuiu a quantidade de valores, para saber quantos ainda faltam
    in = (in + 1) % TAM_BUFFER; //apos a inserção no slot, muda o index para o proximo, quando cheio volta para o inicio 
    slotsDisponiveis++;         //incrementa o numero de slots quando

    //printf("slotsDisponiveis: %d\n",slotsDisponiveis);

    pthread_mutex_unlock(&mutex);
    //printf("libera consumidora\n");
    pthread_cond_signal(&cond_cons); //libera a thread consumidora, agora que tem slot disponivel
}

int * retira(){
    pthread_mutex_lock(&mutex);

    while (slotsDisponiveis == 0){ //caso não tenham slots disponiveis, espera

        if(quantValores==0){        //caso não tenham slots disponiveis e valores sobrando igual a 0, finaliza a thread
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        };

        //printf("consumidora bloqueada\n");
        pthread_cond_wait(&cond_cons, &mutex);  //caso não tenham slots disponiveis, mas ainda valores sobrando, espera.
        //printf("consumidora liberada\n");

    }

    //cria um vetor, aloca memoria e passa o slot para esse vetor
    int * vetor;
    vetor = (int*) malloc(sizeof(int)*N);
    for(int j = 0;j<N;j++){
        vetor[j] = buffer[out*N+j];
    }
    
    
    slotsDisponiveis--;            //diminui a quantidade de slots disponiveis apos a "retirada"
    out = (out + 1) % TAM_BUFFER; // muda o index de retirada para que a proxima thread não retire o mesmo slot
    //printf("consome slot: %d\n",slotsDisponiveis);
    
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_prod); //libera a thread produtora para encher o slot que foi consumido
    return vetor;                    // retorna o slot retirado
}

//funçao "consomeItem?"
void ordenaEscreve(int * vetor){

    int aux; //variavel auxiliar
    //ordenando o ponteiro passado
    for (int contador = 1; contador < N; contador++) {
        for (int i = 0; i < N - 1; i++) {
            if (vetor[i] > vetor[i + 1]) {
                aux = vetor[i];
                vetor[i] = vetor[i + 1];
                vetor[i + 1] = aux;
            }
        }
    }

    //printando o vetor para visualizar (muito utilizado durante a implementação)
    /*for(int i = 0;i<N;i++){
        printf("%d ",vetor[i]);
    }
    printf("\n");*/
    
    //corretude de ordenação, caso o slot que está presentes a ser inserido não esteja ordenado a aplicação para.
    for(int i = 0; i<N-1; i++){
        if(vetor[i]>vetor[i+1]){
            printf("---------------------ERRO CORRETUDE ORDENACAO-------------------\n");
            exit(1);
        }
    }

    pthread_mutex_lock(&mutex_escrita); //exclusão mutua utilizando um mutex especial para inserção concorrente

    //coloca o conteudo do vetor no arquivo
    for(int i = 0; i<N; i++){
        fprintf(arqSaida, "%d" , vetor[i]);
        fputc(' ', arqSaida);
    }
    fputc('\n', arqSaida);

    quantValoresEscritos+=N; //incrementa a quantidade de valores escritos por bloco escrito
    pthread_mutex_unlock(&mutex_escrita);

    pthread_mutex_lock(&mutex);
    //caso seja o ultimo a inserir, libera as thread que estão em espera para finalizarem 
    if(quantValores==0){
        pthread_cond_broadcast(&cond_cons);
        //pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
}


void * consumidor(void * arg){
    int * item;
    while(1){
        item = retira();      //retira um slot
        ordenaEscreve(item);  //ordena e escreve o slot no arquivo

        if(quantValores==0 && slotsDisponiveis == 0){  //caso não tenha mais valores a serem colocados no buffer nem slots disponiveis, finaliza
            pthread_exit(NULL);
        }
    }
}

void * produtor(void * arg){
    while(1){
        insere(); //coloca um slot no buffer

        if(quantValores<=0){     //caso não tenham mais valores a serem inseridos no buffer, finaliza
            pthread_exit(NULL);
        }
    }
}


int main(int argc, char *argv[]){
    double ini, fim; //variaveis para tomada de tempo

    //inicializa as variaveis de sincronizacao
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_escrita, NULL);
    pthread_cond_init(&cond_cons, NULL);
    pthread_cond_init(&cond_prod, NULL);

    
    //validar o numero de entradas
    if (argc < 5) {
        printf("Digite %s < nThreads > < tamBlocos > < arqEntrada > < arqSaida >", argv[0]);
        return 1;
    }

    //passando os valores para variaveis globais
    C = atoi (argv[1]); // numero threads
    N = atoi (argv[2]); // tamBloco
    arqEntrada = fopen(argv[3],"rb"); //abre arquivo de entrada para leitura
    arqSaida = fopen (argv[4],"wb");  //abre arquivo de saida para escrita

    //verifica se ambos os arquivos foram criados com exito
    if (arqEntrada == NULL || arqSaida == NULL){
        printf("Problemas na criação do arquivo\n");
        exit(EXIT_FAILURE);
    }


    fscanf(arqEntrada, "%d" , &quantValores);// pegamos a quantidade de valores no restante do arquivo
    quantValoresTotal = quantValores;        // passamos esse valor para outra variavel para tratamento de condições

     
    //alocando memoria para o buff e buffer auxiliar de inteiros
    buffer = (int *)  malloc(N * 10 * sizeof(int));
    
    //identificadores de threads
    pthread_t tid[P+C];
    int *id[P+C];

    //aloca espaço para os IDs das threads
    for(int i = 0; i < P+C; i++){
        id[i] = malloc(sizeof(int));
        if (id[i] == NULL) exit(-1);
        *id[i] = i+1;
    }


    //inicio tomada tempo
    GET_TIME(ini);

    //cria as threads produtoras
    for (int i = 0; i < P; i++){
        if (pthread_create(&tid[i], NULL, produtor, (void *) id[i])) exit(-1);            
    }

    //cria as threads consumidores
    for (int i = 0; i < C; i++){
        if (pthread_create(&tid[i+P], NULL, consumidor, (void *) id[i+P])) exit(-1);
    }

    //recebendo as threads
    for(long int i = 0; i<C+P; i++){
        if(pthread_join( tid[i],NULL)){
            fprintf(stderr,"ERRO -- pthread_create\n");
            return 3;
        }
    }

    //fim tomada tempo
    GET_TIME(fim);
    printf("Tempo de execução: %lf\n", fim-ini);

    //corretude quantidade de valores escritos, caso não seja igual a primeira linha do arquivo, fecha o programa antes da mensagem de fim
    if(quantValoresEscritos!=quantValoresTotal){
        printf("---------------------ERRO CORRETUDE QUANTIDADE VALORES ESCRITOS-------------------\n");
        exit(1);
    }
    
    printf("Aplicação finalizada com sucesso!\n");

    //liberando as estruturas alocadas
    free(buffer);
    fclose(arqSaida);
    fclose(arqEntrada);
    return 0;
}