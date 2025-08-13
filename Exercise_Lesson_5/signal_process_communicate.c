#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>

// Counts the number of times the signal has been sent
volatile int signal_count = 0;

// signal handling function for child process
void signal_handler(int sig)
{
    if(sig == SIGUSR1)
    {
        printf("Child process received signal from parent.\n");
        signal_count++;
    }
}
int main()
{
    pid_t pid;

    printf("Parent process started (PID: %d).\n", getpid());

    pid = fork(); // Create Child Process

    if(pid < 0)
    {
        // Error creating child process
        perror("fork failed");
        return 1;
    }
    // ---LOGIC FOR CHILD PROCESS---
    if(pid == 0)
    {
        printf("Child process created (PID: %d).\n", getpid());

        // Register signal handler function SIGUSR1
        signal(SIGUSR1, signal_handler);

        // waiting for signal from parent process
        while(signal_count < 5)
        {
            pause();
        }
        printf("Child process exiting after 5 signals.\n");


    }
    else //---LOGIC FOR PARENT PROCESS---
    {
        //Add a small pause to ensure the child process has finished initializing
        sleep(1);
        int i;
        for(i = 0; i < 5; i++)
        {
            printf("Parent process sending signal %d to child (PID: %d)...\n", i + 1, pid);
            kill(pid, SIGUSR1); // Send SIGUSR1 signal to child process
            sleep(2);
        }

        //Wait for child process to finish before parent finishes
        wait(NULL);
        printf("Parent process finished.\n");
    }
    return 0;
}