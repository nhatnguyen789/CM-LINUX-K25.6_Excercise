
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
        printf("CHILD (PID: %d): My original Parent is: %d.\n", getpid(), getppid());

        // Sleep for 2 seconds to make sure Parent is out
        sleep(2);

        printf("CHILD (PID: %d): Now My Parent is: %d, I was adopted!\n", getpid(), getppid());
        printf("I will sleep for 5 more seconds and then it's over.\n");
        sleep(5);
        printf("CHILD: Goodbye!\n");
    }
    else
    {
        // -----PARENT PROCESS-----
        printf("PARENT (PID: %d): I am Parent, I will exit now!\n", getpid());
        // Give your child a definite amount of time to run ahead.
        sleep(1);
        exit(0);
    }
    return 0;
}