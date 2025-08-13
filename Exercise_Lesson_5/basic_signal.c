#include<stdio.h>
#include<stdlib.h>  // for exit()
#include<signal.h>  // for signal()
#include<unistd.h>  // for pause(), getpid()

// Global variable to count the number of times the SIGINT signal is received 
volatile static int sigint_count = 0;

// Signal handler function for SIGINT
// This function will be automatically called by the operating system every time
// the process receives the SIGINT signal (from pressing Ctrl + C)
void sigint_handler(int signum)
{
    sigint_count++;

    // Print out a message every time a signal is received 
    printf("\nSIGINT received. Count: %d\n", sigint_count);
    
    // Flush the buffer to ensure the message is printed immediately
    // This is a good practice when printing in a signal handler
    fflush(stdout);

    // Check if received 3 times then end the program
    if(sigint_count >= 3)
    {
        printf("Received SIGINT 3 times. Exiting program.\n");
        exit(EXIT_SUCCESS); // End the program normally
    }
}
int main()
{
    printf("Programing starting. Process ID (PID): %d \n", getpid());
    printf("Press (Ctrl + C) to send a SIGINT signal.\n");
    printf("The program will exit after the 3rd Ctrl + C.\n");

    // use the signal() function to register a handler function for the SIGINT signal
    // From here, whenever SIGINT is received, the program will call sigint_handler
    // to terminate the program
    signal(SIGINT, sigint_handler);

    // Infinite loop to keep the program "alive" and waiting for a signal
    while(1)
    {
        // pause() will suspend the program until a signal is received
        pause(); 
    }
    return 0;
}