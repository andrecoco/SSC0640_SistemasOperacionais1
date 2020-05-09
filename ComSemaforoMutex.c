#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

//Define o tamanho do Buffer e o numero de objetos a serem produzidos pelo produtor.
#define TAM_BUFFER 10
#define PRODUCAO_MAX 20

/*
Define algumas variáveis:
- 'vazio' e 'cheio' são semáforos
- 'mutex' é um mutex
- 'buffer' é um vetor de inteiros, que será utilizado como buffer pelo produtor e consumidor 
- 'nextP' e 'nextC' são os índices do buffer que determinam aonde o produtor irá produzir, e aonde o consumidor irá consumir 
- 'produzidos' e 'consumidos' são variáveis para guardar o número de objetos que foram produzidos ou consumidos até o momento
*/
sem_t vazio, cheio;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int buffer[TAM_BUFFER], nextP = 0, nextC = 0, produzidos = 0, consumidos = 0;

//Declara as funções do produtor e consumimdor, que serão definidas mais a frente no código
void* producer();
void* consumer();

int main() {
    pthread_t prod, cons;
    
    sem_init(&vazio, 0, TAM_BUFFER);
    sem_init(&cheio, 0, 0);

    pthread_create(&prod, 0, producer, NULL);
    //sleep(5); //descomente para mostrar que o produtor para quando o buffer estiver cheio
    pthread_create(&cons, 0, consumer, NULL);
    
    //Espera as duas threads terminarem
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    //Destroi o mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}

void* producer() {
    while(1){
        sem_wait(&vazio); //Espere o semáforo estar "liberado" (down)
        pthread_mutex_lock(&mutex); //pede a trava do mutex

        //Mexe no Buffer, e ajeita as variáveis
        buffer[nextP] = 1;
        produzidos++;
        printf("Produzi no indice %d, Total Produzido %d\n", nextP, produzidos);
        
        //Muda o indice aonde o proximo objeto sera colocado
        if(nextP == TAM_BUFFER - 1){
            nextP = 0;
        }else{
            nextP++;
        }

        //Se ja produziu tudo, pode finalizar a thread
        if(produzidos == PRODUCAO_MAX){
            printf("Finalizando Produtor!\n");
            pthread_mutex_unlock(&mutex);
            sem_post(&cheio);
            pthread_exit(0);
        }
        
        pthread_mutex_unlock(&mutex); //destrava o mutex
        sem_post(&cheio); //Incrementa o outro semáforo (up)
    }    
    pthread_exit(0);
}

void* consumer() {
    while(1){
        sem_wait(&cheio); //Espere o semáforo estar "liberado" (down)
        pthread_mutex_lock(&mutex); //pede a trava do mutex

        //Mexe no Buffer, e ajeita as variáveis
        buffer[nextC] = 0;
        consumidos++;
        printf("Consumi do indice %d, Total Consumido %d\n", nextC, consumidos);

        //Muda o indice de onde o proximo objeto sera consumido
        if(nextC == TAM_BUFFER - 1){
            nextC = 0;
        }else{
            nextC++;
        }

        //Se ja consumiu tudo, pode finalizar a thread
        if(consumidos == PRODUCAO_MAX){
            printf("Finalizando Consumidor!\n");
            pthread_mutex_unlock(&mutex);
            sem_post(&vazio);
            pthread_exit(0);
        }
        
        pthread_mutex_unlock(&mutex); //destrava o mutex
        sem_post(&vazio); //Incrementa o outro semáforo (up)
    }
    pthread_exit(0);
}