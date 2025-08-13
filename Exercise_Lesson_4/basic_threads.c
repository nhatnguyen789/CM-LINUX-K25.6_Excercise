#include<stdio.h>
#include<stdlib.h>
#include<pthread.h> // Include pthread library for threading
#include<unistd.h> // Include unistd for sleep function

// This is a function that each thread will execute
// it must have a return type of void* and a parameter type of void*
void* thread_function(void* arg)
{
    // get the ID of the running thread itself
    pthread_t thread_id = pthread_self();

    // print the thread ID
    printf(">> Thread with ID %ld is running!\n", (long)thread_id);

    // Simulate a time consuming task
    sleep(1);

    printf(">> Thread with ID %ld has finished!\n", (long)thread_id);

    return NULL; // Return NULL to indicate completion
} 
int main()
{
    // Array to store IDs of 2 threads
    pthread_t thread[2];
    int i;

    printf("MAIN: Prepare to Create 2 threads\n");

    //1. CREATE THREADS
    // Loop to create 2 threads
    for(int i = 0; i < 2; i++)
    {
        if(pthread_create(&thread[i], NULL, thread_function, NULL) != 0)
        {
            perror(">> pthread_create failed");
            return 1; // Exit if thread creation fails
        }
    }

    printf("MAIN: 2 threads created. Now will wait for them to finish.\n");

    //2. WAIT FOR THREADS TO FINISH
    // The main thread must wait for both child threads to finish
    for(int i = 0; i < 2; i++)
    {
        if(pthread_join(thread[i], NULL) != 0)
        {
            perror(">> pthread_join failed");
            return 2; // Exit if waiting for thread fails
        }
    }
    printf("MAIN: Both threads have terminated. Main program exits!.\n");

    return 0; // Return 0 to indicate successful completion
}