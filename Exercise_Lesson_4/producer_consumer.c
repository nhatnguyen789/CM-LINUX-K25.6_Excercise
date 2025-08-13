#include<stdio.h>
#include<stdlib.h>
#include<pthread.h> 
#include<unistd.h>

#define ITEM_COUNT 10

// ---Shared Resource---
int shared_data = 0;    // data storage
int data_ready = 0;     // Flag indicating whether the data is ready
pthread_mutex_t mutex;  // Mutex to protect resources on
pthread_cond_t cond;    // Conditional variables for signaling and waiting
//------------------------

// ---PRODUCER THREAD---
void* producer_thread(void *arg)
{
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        // Lock mutex to start production
        pthread_mutex_lock(&mutex);

        // Create new data (here is the sequence number)
        shared_data = i + 1;
        data_ready = 1; // Set data ready flag
        printf("PRODUCER: Produced data -> %d \n", shared_data);

        // Signal waiting Consumer thread
        pthread_cond_signal(&cond);

        // Unlock mutex
        pthread_mutex_unlock(&mutex);

        // Take a nap to give Consumer time to process
        sleep(1);
    }
    return NULL;
}
//---CONSUMER THREAD---
void* consumer_thread(void* arg)
{
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        // Lock mutex for testing and consumption
        pthread_mutex_lock(&mutex);

        //---THE MOST IMPORTANT PART---
        // if the data is not ready, please wait
        while(!data_ready)
        {
            //pthread_cond_wait will: 
            // 1. Automatically unlock the mutex
            // 2. put this thread to sleep
            // 3. when woken up, it will automatically re-lock the mutex before continuing
            printf("CONSUMER: No data, I will sleep!\n");
            pthread_cond_wait(&cond, &mutex);
            printf("CONSUMER: Awakened!\n");
        }
        // if it runs to here, it means data_ready == 1
        printf("CONSUMER: Data consumed -> %d \n\n", shared_data);
        data_ready = 0; // Reset flag after consumed

        // Unlock mutex
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
int main()
{
    pthread_t prod_tid, cons_tid;
    
    // Initilize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create 2 threads
    pthread_create(&prod_tid, NULL, producer_thread, NULL);
    pthread_create(&cons_tid, NULL, consumer_thread, NULL);

    // Wait for two threads to finish
    pthread_join(prod_tid, NULL);
    pthread_join(cons_tid, NULL);

    //Destroy mutex and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}