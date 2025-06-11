 #include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5  
#define MAX_ITEMS 20   

int buffer[BUFFER_SIZE]; 
int in = 0;              
int out = 0;            

sem_t empty;            
sem_t full;             
pthread_mutex_t mutex;   


void* producer(void* arg) {
    int item;
    for (int i = 0; i < MAX_ITEMS; i++) {
        item = rand() % 100; 

        sem_wait(&empty);   
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("Producer %ld: Inserted %d at position %d\n", (long)arg, item, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex); 
        sem_post(&full);      
    }
    return NULL;
}


void* consumer(void* arg) {
    int item;
    for (int i = 0; i < MAX_ITEMS; i++) {
        sem_wait(&full);     
        pthread_mutex_lock(&mutex); 

        item = buffer[out];
        printf("Consumer %ld: Removed %d from position %d\n", (long)arg, item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex); 
        sem_post(&empty);    
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;

    sem_init(&empty, 0, BUFFER_SIZE); 
    sem_init(&full, 0, 0);         
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&prod_thread, NULL, producer, (void*)1);
    pthread_create(&cons_thread, NULL, consumer, (void*)1);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}