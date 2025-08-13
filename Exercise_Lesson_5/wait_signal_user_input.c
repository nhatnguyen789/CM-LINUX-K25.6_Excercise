#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<sys/select.h>

#define BUFFER_SIZE 1024

// SIGINT signal handling function
void sigint_handler(int sig)
{
    if(sig == SIGINT)
    {
        printf("\nSIGINT received. Continuing...\n");
    }
}
// SIGTERM signal handling function
void sigterm_handler(int sig)
{
    if(sig == SIGTERM)
    {
        printf("\nSIGTERM received. Exiting...\n");
        exit(0);
    }
}
int main()
{
    fd_set read_fds;
    char buffer[BUFFER_SIZE];
    int retval;

    // Register handler functions for signals
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);

    printf("Program is running. You can type something or send signals.\n");
    printf("Press Ctrl + C to send SIGINT.\n");
    printf("Use 'kill <PID>' to send SIGTERM.\n");
    printf("--------------------------------");

    while(1)
    {
        // Always reset and reset fd_set before each call to select()
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds); //STDIN_FILENO is 0

        // select() will wait until there is data on stdin
        retval = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, NULL);

        if(retval == -1)
        {
            perror("select()");
            exit(1);
        }

        // Check if there is data on stdin
        if(FD_ISSET(STDIN_FILENO, &read_fds))
        {
            ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if(bytes_read > 0)
            {
                buffer[bytes_read] = '\0'; // end of string
                printf("You typed: %s", buffer);
            }
        }
    }
    return 0;
}