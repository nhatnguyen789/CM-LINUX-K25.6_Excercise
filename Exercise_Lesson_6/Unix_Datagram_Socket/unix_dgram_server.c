#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>

#define SOCKET_PATH "/tmp/my_unix_dgram.sock"
#define BUFFER_SIZE 1024

int main()
{
    int server_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    const char *reply_message = "The packet has been received by the unix datagram server";

    //---CLEANUP OLD SOCKET FILES---
    unlink(SOCKET_PATH);

    //---STEP 1: CREATE SOCKET ENDPOINT---
    // AF_UNIX: Unix Address family
    // SOCK_DGRAM: Datagram - style, connectionless communication 
    if((server_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    //---STEP 2: ASSIGN ADDRESS (file path) TO SOCKET---
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if(bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) < 0)
    {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    printf("Unix Datagram Server waiting for message at '%s'...\n", SOCKET_PATH);

    //---STEP 3: WAIT FOR DATA FROM CLIENT
    // recvfrom() will wait here, and when it receives data, it will fill the client's address
    // int to the client_addr variable
    int n = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if(n < 0)
    {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';

    printf("Receive packet from client:  %s\n", buffer);

    //---STEP 4: SEND RESPONSE BACK TO CLIENT---
    // Use the address the client just received to send response
    sendto(server_fd, reply_message, strlen(reply_message), 0, (const struct sockaddr*)&client_addr, client_addr_len);
    printf("Sent feedback\n");

    //---STEP 5: CLOSE AND CLEANUP---
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}