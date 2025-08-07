
//--------PROBLEM 1: PROCESS INITIALIZATION AND CLEAN UP--------

#include<stdio.h>
#include<stdlib.h> // for  exit()
#include<unistd.h> // for fork(), getpid(), getppid()
#include<sys/types.h> // for pid_t 
#include<sys/wait.h> // for wait() and macros WIFEXITED(), WEXITSTATUS().

int main()
{
    pid_t pid; // Data type of process IDs 
    int child_status; // Variable to hold the status of the child process

    // Create a child process
    pid = fork();

    // Based on the returned 'pid' value to classify the stream
    if(pid < 0)
    {
        // Fork failed
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }
    else if(pid == 0)
    {
        //--------CHILD PROCESS--------
        printf("CHILD: Hello, I am the child process!\n");
        printf("CHILD: My PID is: %d \n", getpid());
        printf("CHILD: I will terminate and return an exit code of 10.\n");
        exit(10); // Child process exits with code 10 ;
    }
    else
    {
        //--------PARENT PROCESS--------
        printf("PARENT: Hello, I am the parent process!\n");
        printf("PARENT: My PID is: %d \n", getpid());
        printf("PARENT: My child's PID is: %d \n", pid);
        printf("PARENT: Waiting for the child process to terminate...\n");

        // Wait for the child process and retrieve its exit status 
        wait(&child_status);

        // Check if the child process terninated normally
        if(WIFEXITED(child_status))
        {
            // if have exited normally, retrive the exit code
            int exit_code = WEXITSTATUS(child_status);
            printf("PARENT: Child process terminated normally with exit code: %d\n", exit_code);
        }
        else
        {
            // if the child process did not terminate normally
            printf("PARENT: Child process did not terminate normally.\n");
        }
        printf("PARENT: Work is done, now I'll finish too!\n");
    }

}


