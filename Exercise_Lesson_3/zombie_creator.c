
//--------PROBLEM 3: SURVEY OF SPECIAL PROCESS STATES--------

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
    pid_t pid = fork();

    if(pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        // -----CHILD PROCESS-----
        printf("CHILD (PID: %d): I will end it now.\n", getpid());
        exit(0); // Child is finish
    }
    else
    {
        // -----PARENT PROCESS-----
        printf("PARENT (PID: %d): I created Child (PID: %d).\n", getpid(), pid);
        printf("PARENT: I won't call wait() instead I'll sleep for 30 seconds.\n");


        // Parent intentionally didn't "clean up" child but took a long nap
        sleep(30);

        printf("PARENT: I'm awake and it's over. The zombies will be cleared.\n");
    }
    return 0;
}