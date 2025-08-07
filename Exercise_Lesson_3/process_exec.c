
//--------PROBLEM 2: REPLACING EXECUTABLE CODE AND INTERACTING WITH THE ENVIROMENT--------

#include<stdio.h>
#include<stdlib.h> // for exit()
#include<unistd.h> // for fork(), getpid(), getppid()
#include<sys/types.h> // for pid_t
#include<sys/wait.h> // for wait() and macros WIFEXITED(), WEXITSTATUS()

int main()
{
    pid_t pid; // Data type of process IDs 

    // 1. PARRNT ESTABLISHES ENVIROMENT VARIABLES BEFORE FORK
    // This enviroment variable will be inherited by the child process
    if(setenv("MY_COMMAND", "ls", 1) != 0)
    {
        perror("setenv");
        exit(1);
    }
    if(setenv("MY_ARGUMENT", "-l", 1) != 0)
    {
        perror("setenv");
        exit(1);
    }

    // Create a child process
    pid = fork();

    if(pid < 0)
    {
        // ERROR: Fork failed
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        // --------CHILD PROCESS--------
        printf("CHILD: Hello, I am the child process, I will read the environment variable and execute the command!\n");

        // 2. CHILD READ ENVIRONMENT VARIABLES
        const char *command = getenv("MY_COMMAND");
        const char *argument = getenv("MY_ARGUMENT");
        if(command == NULL || argument == NULL)
        {
            fprintf(stderr, "CHILD: Enviroment variables MY_COMMAND or MY_ARGUMENT not found.\n");
            exit(1); 
        }
        printf("CHILD: Command found is: %s %s, Preparing to execute ... \n\n", command, argument);

        // 3. CHILD USES EXEC TO REPLACE CODE
        // The child process will be transformed into the "ls -l" command
        execlp(command, command, argument, NULL);

        // THIS LINE WILL NEVER RUN IF EXECLP SUCCEEDS
        // it only runs if the 'ls' command does not exist on the system
        perror("execlp");
        exit(1); // exit if execlp is failure
    }
    else
    {
        //------PARENT PROCESS------
        printf("PARENT: I am Parent. I will wait for my child to complete the task.\n");

        // Parent just wait for child to finish
        wait(NULL);
        printf("\nPARENT: Child have finished executing. The program is terminated.\n");
    }
    return 0;
}