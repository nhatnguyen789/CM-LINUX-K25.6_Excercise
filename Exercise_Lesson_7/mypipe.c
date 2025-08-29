#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>


int main()
{
    //Array to contain 2 file descriptor of the pipe: pipefd[0] is the reader, 
    // pipefd[1] is the writer
    int pipefd[2];
    pid_t pid1, pid2; 

    //---STEP 1: CREATE PIPE---
    //Create a pipe. If it fails, throw an error and exit.
    if(pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //---STEP 2: CREATE CHILD PROCESS 1 (WILL RUN: 'ls -l')---
    pid1 = fork();

    if(pid1 < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if(pid1 == 0)
    {
        //---This is the code of Child process 1 ('ls -l')

        //Close the read end of the pipe because this process is write only
        close(pipefd[0]);

        //Redirect stdout (file desciptor 1) to the pipe's write end (pipefd[1]).
        //Anything 'ls -l' prints to the screen (stdout) will now be written to the pipe
        dup2(pipefd[1], STDOUT_FILENO);

        //Once copied, the original recoder is no longer needed.
        close(pipefd[1]);

        //Execute the command 'ls -l'. This is process will be completely replaced by 'ls -l'
        execlp("ls", "ls", "-l", NULL);

        //This line is only executed if execlp() fails
        perror("execlp ls-l");
        exit(EXIT_FAILURE);
    }

    //---STEP 3: CREATE CHILD PROCESS 2 (WILL RUN 'wc -l')---
    //This code is still in the original parent process.
    pid2 = fork();
    if(pid2 < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if(pid2 == 0)
    {
        //---This is the code of Child Process 2 ('wc -l')---

        // Close the pipe's write end since this process is read-only
        close(pipefd[1]);

        // Redirect stdin (file descriptor 0) to the pipe's input (pipefd[0]).
        // The 'wc -l' command will now read input from the pipe instead of from the keyboard (stdin)
        dup2(pipefd[0], STDIN_FILENO);

        //Once copied, the original reader is no longer needed
        close(pipefd[0]);

        //Execute the command 'wc -l'
        execlp("wc", "wc", "-l", NULL);

        //This line is only executed if execlp() fails
        perror("execlp wc-l");
        exit(EXIT_FAILURE);
    }

    //---STEP 4: PARENT PROCESS CODE---
    // The parent does not read or write directly to the pipe, so it must close both ends
    // **This is a VERY IMPORTANT step!**
    close(pipefd[0]);
    close(pipefd[1]);

    //Wait for both child processes to finish to avoid zombie processes
    //wait(): Wait for any child process to exit and return that process's process ID along with its exit status.
    //waitpid(): Provides the ability to specify which child process the parent process wants to wait for via the pid argument.
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0); 

    printf("The two child processes have completed!\n");

    return 0;
}