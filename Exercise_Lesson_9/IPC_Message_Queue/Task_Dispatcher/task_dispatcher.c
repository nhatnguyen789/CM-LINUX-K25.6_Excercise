#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mqueue.h>   /*For message queue function*/
#include<fcntl.h>      /*For O_* constants*/

//The name of the queue must be indentical to the name in the task_worker 
const char* QUEUE_NAME = "/my_task_queue";

int main(int argc, char* argv[])
{
    mqd_t mq_desc; //Message queue indentifier

    //---Step 1: Open the message queue---
    //The program needs exactly 3 parameters: ./program name <priority> "<message>""
    if(argc != 3)
    {
        fprintf(stderr, "Usage syntax: %s <priority> \"<task_description>\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Convert priority from string to integer 
    unsigned int priority = atoi(argv[1]);
    const char* task_content = argv[2];

    //---Step 2: Open an existing message queue---
    // O_WRONLY: Only write (send) messages to queue
    // This function will fail if the worker has not run and created the queue
    mq_desc = mq_open(QUEUE_NAME, O_WRONLY);
    if(mq_desc == (mqd_t)-1)
    {
        perror("mq_open");
        fprintf(stderr, "Error: Make sure tasK_worker is running.\n");
        exit(EXIT_FAILURE);
    }

    //---Step 3: Send the task to the queue---
    // The mq_send function requires:
    // 1. Queue identifier
    // 2. Pointer to the message content
    // 3. Message size (in bytes)
    // 4. Message priority (0 to 31, higher number means higher priority)
    if(mq_send(mq_desc, task_content, strlen(task_content), priority) == -1)
    {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    printf("The task was successfully submitted with priority %u.\n", priority);

    //---Step 4: Close connection to queue---
    // Only close the identidier of this process, do not delete the queue from the system
    mq_close(mq_desc);

    return 0;
}