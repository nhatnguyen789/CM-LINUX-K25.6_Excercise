#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define PORT 8080

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    char buffer[1024] = {0};
    const char *reply_message = "Message received by server!";
    
    // -----STEP 1: RANDOM SOCKET ENDPOINT-----
    // AF_INET: Using IPv4 address family 
    // SOCK_STREAM: Use stream socket type (TCP), ensure reliable data
    // 0 : Use default protocol for SOCK_STREAM is TCP
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // -----STEP 2: ASSIGN ADDRESS (IP + PORT) TO SOCKET-----
    server_addr.sin_family = AF_INET;   // IPv4 address family
    // INADDR_ANY: Accept connections from any IP address on the machine
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // htons: Convert port number from machine byte order to network byte order
    server_addr.sin_port = htons(PORT);
    
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //-----STEP 3: LISTENING FOR INCOMING CONNECTIONS-----
    //3. Maximum number of connections that can wait in the queue
    if(listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on PORT: %d ...\n", PORT);

    //-----STEP 4: ACCEPT A CONNECTION FROM THE CLIENT-----
    // accept() is a blocking function, the program will stop here until a client connects
    // It return a new socket descriptor to communicate with the just connected client
    if((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from client!");

    //-----STEP 5: READ DATA FROM CLIENT-----
    read(new_socket, buffer, 1024);
    printf("Client say: %s\n", buffer);

    //-----STEP 6: SEND FEEDBACK BACK TO CLIENT-----
    send(new_socket, reply_message, strlen(reply_message), 0);
    printf("Sent feedback message to customer!");

    //-----STEP 7: CLOSE CONNECTION-----
    close(new_socket);  // Close the communication socket with this client
    close(server_fd);

    return 0;
}