#include<stdio.h>
#include<stdlib.h>
#include<pthread.h> // Include pthread library for threading

#define NUM_THREADS 3
#define INCREMENT_LIMIT 1000000

// Global variables, which are resource shared between threads
long long counter = 0; // Shared counter

// Mutex "lock" to protect the shared resource
pthread_mutex_t mutex;

// Job function for each thread 
void* increment_counter(void* arg)
{
    for(int i = 0; i < INCREMENT_LIMIT; i++)
    {
        // ---START OF GANG AREA (CRITICAL SECTION)---

        // 1. Lock the mutex before accessing the shared resource
        pthread_mutex_lock(&mutex);

        // only one thread is allowed to access the shared resource at a time
        counter++;

        // 2. Unlock immediately upon completion
        pthread_mutex_unlock(&mutex);
    }
    return NULL; // Return NULL to indicate completion
}
int main()
{
    pthread_t threads[NUM_THREADS]; // Array to hold thread IDs
    int i;

    // Initialize mutex before use
    if(pthread_mutex_init(&mutex, NULL) != 0)
    {
        perror(">> pthread mutex_init failed");
        return 1; // Exit if mutex initialization fails
    }

    // Create 3 threads
    for(i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, increment_counter, NULL);
    }

    // Wait for 3 threads to finish
    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Destroy the mutex after use
    pthread_mutex_destroy(&mutex);

    // Print the final value of the counter
    printf("Final counter value: %lld\n", counter);

    return 0; // Return 0 to indicate successful completion
}