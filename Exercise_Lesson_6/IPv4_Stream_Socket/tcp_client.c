#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define PORT 8080   // The PORT of the server to connect to
#define SERVER_IP "127.0.0.1"   //Server IP address (localhost)

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    const char *message = "Hello from client";

    //-----STEP 1: CREATE SOCKET ENDPOINT-----
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    //memset(): Make sure that the entire sockaddr_in struct is initialized cleanly, without any garbage data,
    //so that when assigning the server address fields, 
    //the value passed to the connect() function is completely correct.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    //-----STEP 2: CONVERT IP ADDRESS FROM STRING TO BINARY-----
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("invalid address/ address not supported");
        exit(EXIT_FAILURE);
    }

    //-----STEP 3: CONNECT TO SERVER-----
    if(connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!");

    //-----STEP 4: SEND DATA TO SERVER-----
    send(client_fd, message, strlen(message), 0);
    printf("Message sent to server!");

    //-----STEP 5: READ RESPONSE FROM SERVER-----
    read(client_fd, buffer, 1024);
    printf("Server response: %s\n", buffer);

    //-----STEP 6: CLOSE CONNECTION-----
    close(client_fd);

    return 0;
}