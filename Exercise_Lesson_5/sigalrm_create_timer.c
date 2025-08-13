#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

//Global variable to store the number of seconds elapsed 
volatile static int seconds_elapsed = 0;

// Signal handler for SIGALRM 
void sigalrm_handler(int signum)
{
    seconds_elapsed++;
    printf("Timer: %d second \n", seconds_elapsed);
    fflush(stdout);

    // Stop condition: end program after 10 seconds 
    if(seconds_elapsed >= 10)
    {
        printf("Timer finished after 10 seconds. Exiting.\n");
        exit(EXIT_SUCCESS);
    }

    // RE-ACTIVATE ALARM: This is the key step 
    // Schedule the kernel to send another SIGALRM signal in 1 second
    alarm(1);
}
int main()
{
    printf("Starting timer program. PID: %d\n", getpid());
    printf("A SIGALRM will be triggered every second.\n");

    // Register a handler to handle the SIGALRM signal
    signal(SIGALRM, sigalrm_handler);

    // Trigger the alarm for the first time
    // Requires the kernel to send the first SIGALRM signal after 1 second
    alarm(1);

    // Put the process into hibernation, waiting for a signal.
    // The pause() system call will only return when a signal handler has finished executing.
    while(1)
    {
        pause();
    }

    return 0;
}