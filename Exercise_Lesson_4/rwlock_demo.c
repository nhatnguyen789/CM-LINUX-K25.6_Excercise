#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

// Shared resource 
int shared_resource = 100;

// lock Read-Write
pthread_rwlock_t rwlock;

// Work function for reader thread
void* reader_thread(void* arg)
{
    long id = (long)arg;

    // Read lock required
    pthread_rwlock_rdlock(&rwlock);

    // ---START READING AREA AT THE SAME TIME---
    printf("Reader %ld: Start Reading... Value is: %d \n", id, shared_resource);
    // Simulate time - Consuming reading
    sleep(1);
    printf("Reader %ld: Read Completion!\n", id);
    //---STOP READING AREA AT THE SAME TIME---

    // Unlock
    pthread_rwlock_unlock(&rwlock);

    return NULL;
}
// work function for writer thread
void* writer_thread(void* arg)
{
    long id = (long)arg;

    // Write lock required
    pthread_rwlock_wrlock(&rwlock);

    //---START EXCLUSIVE WRITTING AREA---
    printf("writer %ld: Start writting...\n", id);
    shared_resource++;
    printf("Writer %ld: Done writting. New value is %d.\n", id, shared_resource);
    //---END OF EXCLUSIVE WRITTING AREA---
    //Unlock
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
int main()
{
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    long i;

    // Initialize Read-Write Lock
    pthread_rwlock_init(&rwlock, NULL);

    // Create Writting threads
    for(int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_create(&writers[i], NULL, writer_thread, (void*)(intptr_t)(i + 1));
    }

    // Create Reading threads
    for(int i = 0; i < NUM_READERS; i++)
    {
        pthread_create(&readers[i], NULL, reader_thread, (void*)(intptr_t)(i + 1));
    }

    // Wait for Threads to finish 
    for(int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }
    for(int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    //Destroy Lock
    pthread_rwlock_destroy(&rwlock);

    printf("\nProgram end. Final value: %d\n", shared_resource);

    return 0;
}