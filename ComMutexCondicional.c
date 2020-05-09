#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 10 //tamanho maximo do buffer


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //inicializa o mutex
pthread_cond_t condc, condp;
int buffer[MAX], next = 0; //buffer e proximo endereco disponivel

void* producer();
void* consumer();


int main() {
    pthread_t prod, cons;

    pthread_cond_init(&condc, NULL);
    pthread_cond_init(&condp, NULL);

    pthread_create(&cons, 0, consumer, NULL);
    pthread_create(&prod, 0, producer, NULL);

    //sincroniza as threads
    pthread_join(cons, NULL);
    pthread_join(prod, NULL);
    

    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&mutex);

    return 1;
}


void* producer() {
    for (int i = 0; i < MAX; i++) {
    	//dá lock para garantir acesso exclusivo a regiao critica
        pthread_mutex_lock(&mutex);

        while (next != 0) {
        	//se a variavel ja foi alterada, esperada a thread do consumidor
            pthread_cond_wait(&condp, &mutex);
            pthread_cond_signal(&condc);
        }
        
        //introduz no buffer
        buffer[i] = 1;
        next++;
        printf("Produziu no %d\n", i+1);

        //unlocka a regiao critica
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutex);
        
    }

    pthread_exit(0);
}


void* consumer() {
    for (int i = 0; i < MAX; i++) {
    	//locka a regiao critica para garantir acesso exclusivo
        pthread_mutex_lock(&mutex);
        //sleep(3);

        while (next == 0) {
        	//se a variavel ja foi consumida, esperada a thread do produtor
            pthread_cond_wait(&condc, &mutex);
            pthread_cond_signal(&condp);
        }

        //consome do buffer
        buffer[i] = 0;
        next--;
        printf("Consumiu no %d\n", i+1);

        //unlocka a regiao critica
        pthread_cond_signal(&condp);
        pthread_mutex_unlock(&mutex);
        
    }

    pthread_exit(0);
}