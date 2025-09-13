#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>      /*For O_* constants*/
#include<sys/stat.h>   /*For mode constants*/
#include<mqueue.h>     /*For message queue function*/
#include<signal.h>    /*For signal handling*/

//---Global Variables and constants---

//Defines the name of the queue, starting with a '/'
const char* QUEUE_NAME = "/my_task_queue";
#define MAX_MSG_SIZE 8192 // Maximum size of a message
#define MAX_MSG_COUNT 10 // Maximum number of messages in the queue

//Global variable to store queue indentifier, making it accessible in signal handlers
mqd_t mq_desc;

//---Cleanup function when receiving Ctrl+C signal---
//This function will be called when the user presses Ctrl+C (SIGINT signal)
void cleanup_handler(int signum)
{
    printf("\nCtrl+C pressed. Cleaning up and exiting...\n");

    //Close the message queue identifier
    if(mq_close(mq_desc) == -1)
    {
        perror("mq_close");
    }

    //Delete the message queue from the system. Without this step, the queue will exist forever.
    if(mq_unlink(QUEUE_NAME) == -1)
    {
        perror("mq_unlink");
    }
    
    exit(0);
}

int main()
{
    struct mq_attr attr; //Struct to define properties for queue
    char buffer[MAX_MSG_SIZE + 1]; // Buffer to receive messages 
    unsigned int priority; // Varibale to get message priority

    //---Step 1: Set up Signal Handler---
    //Register cleanup_handler function to handle SIGINT signal 
    signal(SIGINT, cleanup_handler);

    //---Step 2: Define properties for queue---
    attr.mq_flags = 0; // Default
    attr.mq_maxmsg = MAX_MSG_COUNT; //Maximum number of messages 
    attr.mq_msgsize = MAX_MSG_SIZE; //Maximum size of each message

    //---Step 3: Create or open a message queue---
    // O_CREAT: Create queue if it does not exist
    // O_RDONLY: Only read (receive) messages from queue
    // 0644: File-system access permissions for queue
    // &attr: Pointer to defined attributes struct 
    mq_unlink(QUEUE_NAME); // Remove the queue if it already exists
    mq_desc = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if(mq_desc == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Task Worker is ready. Waiting for tasks...(Pres Ctrl+C to exit)\n");

    //---Step 4: Main loop to receive and process tasks---
    while(1)
    {
        //Wait and receive a message. This is a blocking function.
        //It will automatically pick the message with the highest priority 
        ssize_t bytes_read = mq_receive(mq_desc, buffer, MAX_MSG_SIZE, &priority);
        if(bytes_read >= 0)
        {
            buffer[bytes_read] = '\0'; // Make sure the string is null-terminated

            //Print out the content and priority of the task
            printf("Processing task (Priority: %u) : %s\n", priority, buffer);

            //Simulate time-consuming processing
            sleep(1);
        }
        else
        {
            perror("mq_receive");
        }
    }

    // The code here will be never be run because the program exist 
    //using the signal handler. But cleanup should still be done there.
    return 0; 
}
